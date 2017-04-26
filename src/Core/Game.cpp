#include <FLIGHT/Core/Game.hpp>

namespace FLIGHT {
// I was getting kernel panics on macOS when trying to draw
// to an offscreen framebuffer before having displayed the window
// at least once.
void Patch::SubvertMacOSKernelPanics(Game & game) {
    sf::Event event;
    while (game.m_window.pollEvent(event))
        ;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    game.m_window.display();
}

// Rendering anything before having once drawn something to the shadowmap was
// causing glErrors. I'm not sure why this is, it could be that I forgot to
// unbind something somewhere, but the source of the issue just isn't clear to
// me.
void Patch::FixMysteriousStateGlitch(Game & game) {
    auto dummy =
        std::make_shared<Plane>(game.m_planesRegistry["RedTail"], "RedTail");
    game.m_renderer->Dispatch(*dummy);
}

void Game::SetSeed(const time_t seed) { m_seed = seed; }

time_t Game::GetSeed() const { return m_seed; }

void Game::UpdateEntities(const Time dt) {
    std::lock_guard<std::recursive_mutex> lk(m_entityList.mutex);
    for (auto it = m_entityList.list.begin();
         it not_eq m_entityList.list.end();) {
        if ((*it)->GetDeallocFlag()) {
            it = m_entityList.list.erase(it);
        } else {
            (*it)->Update(dt);
            ++it;
        }
    }
}

void Game::AutoAssignController(Player & player) {
    if (not m_unassignedGamepads.empty()) {
        player.GiveController(std::move(m_unassignedGamepads.back()));
        m_unassignedGamepads.pop_back();
    } else if (m_unassignedMouseJSController) {
        auto dm = sf::VideoMode::getDesktopMode();
        sf::Mouse::setPosition(sf::Vector2<int>(dm.width / 2, dm.height / 2));
        player.GiveController(std::move(m_unassignedMouseJSController));
    }
}

void Game::PollEvents() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        switch (event.type) {
        case sf::Event::Closed:
            m_running = false;
            break;

        case sf::Event::MouseMoved:
        case sf::Event::JoystickMoved: {
            auto * p1Controller = m_player1.GetController();
            if (p1Controller) {
                p1Controller->GetJoystick().Update(event);
            }
        } break;

        case sf::Event::JoystickButtonPressed:
        case sf::Event::JoystickButtonReleased:
        case sf::Event::KeyReleased:
        case sf::Event::KeyPressed: {
            auto * p1Controller = m_player1.GetController();
            if (p1Controller) {
                p1Controller->GetButtonSet().Update(event);
            }
        } break;

        case sf::Event::JoystickConnected:
            TryBindGamepad(sf::Joystick::getIdentification(
                               event.joystickConnect.joystickId),
                           event.joystickConnect.joystickId);
            break;

        case sf::Event::JoystickDisconnected: {
            auto * p1Controller = m_player1.GetController();
            if (p1Controller) {
                if (auto id = p1Controller->GetId()) {
                    if (*id == event.joystickConnect.joystickId) {
                        auto controller = m_player1.TakeController();
                        AutoAssignController(m_player1);
                    }
                }
            }
            for (auto it = m_unassignedGamepads.begin();
                 it != m_unassignedGamepads.end();) {
                if (*(*it)->GetId() == event.joystickConnect.joystickId) {
                    it = m_unassignedGamepads.erase(it);
                } else {
                    ++it;
                }
            }
        } break;

        case sf::Event::GainedFocus:
            break;

        case sf::Event::Resized:
            throw std::runtime_error("Resizing window unsupported");
            break;

        case sf::Event::LostFocus: {
            // TODO... go to menu?
        } break;

        // Unused events here. I generally don't like the default keyword, and
        // the SFML API could change in the future to include more events that I
        // want to be warned about.
        case sf::Event::MouseWheelMoved:
        case sf::Event::MouseWheelScrolled:
        case sf::Event::TextEntered:
        case sf::Event::MouseButtonPressed:
        case sf::Event::MouseButtonReleased:
        case sf::Event::MouseEntered:
        case sf::Event::MouseLeft:
        case sf::Event::TouchBegan:
        case sf::Event::TouchMoved:
        case sf::Event::TouchEnded:
        case sf::Event::SensorChanged:
            break;

        case sf::Event::Count:
            throw std::runtime_error("WTF??? Something went horribly wrong");
        }
    }
}

ConfigData & Game::GetConf() { return m_conf; }

AssetManager & Game::GetAssetMgr() { return m_assetManager; }

void Game::TryBindGamepad(const sf::Joystick::Identification & ident,
                          const unsigned id) {
    auto jsBtnMap = std::find_if(
        m_conf.controls.gamepadMappings.begin(),
        m_conf.controls.gamepadMappings.end(),
        [&ident](const ConfigData::ControlsConf::GamepadMapping & mapping) {
            return mapping.vendorId == ident.vendorId and
                   mapping.productId == ident.productId;
        });
    if (jsBtnMap not_eq m_conf.controls.gamepadMappings.end()) {
        m_unassignedGamepads.push_back(
            std::unique_ptr<Controller>(new GamepadInput(*jsBtnMap, id)));
    }
}

void Game::InitJoysticks() {
    for (int i = 0; i < sf::Joystick::Count; ++i) {
        TryBindGamepad(sf::Joystick::getIdentification(i), i);
    }
}

void Game::Configure(const ConfigData & conf) {
    try {
        m_window.create(sf::VideoMode::getDesktopMode(),
                        conf.localization.strings.appName,
                        sf::Style::Fullscreen,
                        sf::ContextSettings(24, 8, conf.graphics.antialiasing,
                                            3, 3, sf::Style::Default, false));
        m_conf = conf;
        m_unassignedMouseJSController = std::unique_ptr<Controller>(
            new KeyboardMouseInput(m_conf.controls.keyboardMapping));
        InitJoysticks();
        m_planesRegistry = LoadPlanes();
        m_terrainManager =
            std::unique_ptr<TerrainManager>(new MountainousTerrain);
        m_camera = std::unique_ptr<Camera>(new PlaneCamera);
        m_renderer = std::unique_ptr<DisplayImpl>(new OpenGLDisplayImpl);
        AutoAssignController(m_player1);
        m_window.setMouseCursorVisible(not conf.graphics.hideCursor);
        m_window.setVerticalSyncEnabled(conf.graphics.vsyncEnabled);
        m_assetManager.LoadResources();
        Patch::SubvertMacOSKernelPanics(*this);
        Patch::FixMysteriousStateGlitch(*this);
        m_sceneStack.stack.push(std::make_shared<CreditsScreen>());
    } catch (const std::exception & ex) {
        m_window.close();
        throw std::runtime_error(ex.what());
    }
}

Game::Game()
    : m_running(false), m_seed(time(nullptr)), m_restartRequested(false) {}

PlaneRegistry & Game::GetPlaneRegistry() { return m_planesRegistry; }

void Game::NotifyThreadExceptionOccurred(std::exception_ptr ex) {
    std::lock_guard<std::mutex> lk(m_threadExceptions.mutex);
    m_threadExceptions.excepts.push_back(ex);
}

void Game::Restart() {
    while (not m_sceneStack.stack.empty()) {
        m_sceneStack.stack.pop();
    }
    m_entityList.list.clear();
    m_terrainManager = std::unique_ptr<TerrainManager>(new MountainousTerrain);
    m_sceneStack.stack.push(std::make_shared<CreditsScreen>());
}

static const auto g_saveFileName = "Save.xml";

void Game::RemoveSaveData() {
    std::remove((ResourcePath() + g_saveFileName).c_str());
}

void Game::Save() {
    XMLSerializer serializer;
    serializer.Dispatch(*this);
    serializer.Dispatch(m_player1);
    serializer.PushRoot("Entities");
    {
        std::lock_guard<std::recursive_mutex> lk(m_entityList.mutex);
        for (auto & entity : m_entityList.list) {
            entity->Serialize(serializer);
        }
    }
    serializer.PopRoot();
    std::fstream out(ResourcePath() + g_saveFileName, std::ios::out);
    serializer.Dump(out);
    m_running = false;
}

void Game::SaveAndQuit() {
    this->Save();
    m_running = false;
}

void Game::RestoreFromSave() {
    pugi::xml_document doc;
    auto res = doc.load_file((ResourcePath() + g_saveFileName).c_str());
    if (not res) {
        throw std::runtime_error("Error loading save file: " +
                                 std::string(res.description()));
    }
    auto root = *doc.begin();
    auto header = root.child("Header");
    m_seed = std::stoi(header.child("Seed").child_value());
    auto player = root.child("Player");
    m_player1.SetScore(std::stoi(player.child_value("Score")));
    unsigned playerPlaneRID =
        player.child("Plane").attribute("refid").as_uint();
    for (auto entity : root.child("Entities")) {
        if (strcmp(entity.name(), "Plane") == 0) {
            const std::string blueprintName = entity.attribute("class").value();
            auto plane = CreateSolid<Plane>(m_planesRegistry[blueprintName],
                                            blueprintName);
            plane->SetPosition(
                {std::stof(entity.child("Position").child_value("x")),
                 std::stof(entity.child("Position").child_value("y")),
                 std::stof(entity.child("Position").child_value("z"))});
            plane->SetRotation(
                {std::stof(entity.child("Rotation").child_value("x")),
                 std::stof(entity.child("Rotation").child_value("y")),
                 std::stof(entity.child("Rotation").child_value("z"))});
            unsigned id = entity.attribute("id").as_uint();
            if (id == playerPlaneRID) {
                m_player1.GivePlane(plane);
                GetCamera().SetTarget(plane);
                playerPlaneRID = 0;
            }
        }
    }
    if (playerPlaneRID) {
        throw std::runtime_error("Corrupt save file");
    }
}

void Game::LogicLoop() {
    sf::Clock clock;
    try {
        while (m_running) {
            UpdateCap<1000> cap;
            auto dt = clock.restart();
            this->m_smoothDTProv.Feed(dt.asMicroseconds());
            std::shared_ptr<Scene> currentScene;
            {
                std::lock_guard<std::mutex> lk(this->m_sceneStack.mutex);
                currentScene = m_sceneStack.stack.top();
                this->m_sceneStack.stack.top()->UpdateState(
                    this->m_sceneStack.stack);
            }
            currentScene->UpdateLogic(m_smoothDTProv.Get());
            if (GAMEFEEL::WasPaused()) {
                clock.restart();
                GAMEFEEL::Reset();
            }
        }
    } catch (const std::exception &) {
        this->NotifyThreadExceptionOccurred(std::current_exception());
    }
}

void Game::RequestRestart() { m_restartRequested = true; }

void Game::Run() {
    assert(not m_conf.empty);
    m_running = true;
    ThreadGuard logicThreadGrd(&Game::LogicLoop, this);
    try {
        while (m_running) {
            PollEvents();
            std::shared_ptr<Scene> currentScene;
            {
                // NOTE: Because the logic thread might pop a scene
                // while this thread is in the middle of a Scene::Display()
                // call.
                std::lock_guard<std::mutex> lk(m_sceneStack.mutex);
                if (m_restartRequested) {
                    Restart();
                    m_restartRequested = false;
                }
                currentScene = m_sceneStack.stack.top();
            }
            currentScene->Display(*m_renderer);
            m_window.display();
            AssertGLStatus("graphics loop");
            if (not m_threadExceptions.excepts.empty()) {
                std::rethrow_exception(m_threadExceptions.excepts.front());
            }
        }
    } catch (const std::exception & ex) {
        m_running = false;
        m_window.close();
        throw std::runtime_error(ex.what());
    }
}

SkyManager & Game::GetSkyMgr() { return m_skyManager; }

TerrainManager & Game::GetTerrainMgr() { return *m_terrainManager.get(); }

CollisionManager & Game::GetCollisionMgr() { return m_collisionManager; }

Camera & Game::GetCamera() {
    if (not m_camera) {
        throw std::runtime_error("Camera not set");
    }
    return *m_camera.get();
}

void Game::SetCamera(std::unique_ptr<Camera> camera) {
    m_camera = std::move(camera);
}

Player & Game::GetPlayer1() { return m_player1; }

sf::Vector2<unsigned> Game::GetSubwindowSize() const {
    return m_window.getSize();
}

bool Game::IsRunning() const { return m_running; }
}
