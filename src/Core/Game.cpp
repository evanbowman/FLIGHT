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
void Patch::ShadowMapPreliminarySweep(Game & game) {
    auto dummy = std::make_shared<Plane>(game.m_planesRegistry["RedTail"]);
    game.m_player.GivePlane(dummy);
    game.DrawShadowMap();
}

void Game::SetSeed(const time_t seed) { m_seed = seed; }

time_t Game::GetSeed() const { return m_seed; }

void Game::UpdateEntities(const Time dt) {
    std::lock_guard<std::recursive_mutex> lk(m_entitiesMtx);
    for (auto it = m_entities.begin(); it != m_entities.end();) {
        if ((*it)->GetDeallocFlag()) {
            it = m_entities.erase(it);
        } else {
            (*it)->Update(dt);
            ++it;
        }
    }
}

void Game::SetupShadowMap() {
    glGenFramebuffers(1, &m_shadowMapFB);
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFB);
    glGenTextures(1, &m_shadowMapTxtr);
    glBindTexture(GL_TEXTURE_2D, m_shadowMapTxtr);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_window.getSize().x / 2,
                 m_window.getSize().y / 2, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                 nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           m_shadowMapTxtr, 0);
    glDrawBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Unable to set up frame buffer");
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::PollEvents() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        switch (event.type) {
        case sf::Event::Closed:
            m_running = false;
            break;

        case sf::Event::JoystickMoved:
        case sf::Event::MouseMoved:
            m_input.joystick->Update(event);
            break;

        case sf::Event::JoystickButtonPressed:
        case sf::Event::JoystickButtonReleased:
        case sf::Event::KeyReleased:
        case sf::Event::KeyPressed:
            m_input.buttonSet->Update(event);
            break;

        case sf::Event::JoystickConnected:
            if (event.joystickConnect.joystickId == 0) {
                TryBindGamepad(sf::Joystick::getIdentification(0));
            }
            break;

        case sf::Event::JoystickDisconnected:
            if (event.joystickConnect.joystickId == 0) {
                m_input.joystick = std::unique_ptr<Joystick>(new MouseJoystickProxy);
                m_input.buttonSet = std::unique_ptr<ButtonSet>(new KeyboardButtonSet(
                    m_conf.controls.keyboardMapping));
            }
            break;

        case sf::Event::GainedFocus:
            break;

        case sf::Event::Resized:
            throw std::runtime_error("Resizing window unsupported");
            break;

        case sf::Event::LostFocus: {
            std::lock_guard<std::mutex> lk(m_sceneStackMtx);
            if (std::dynamic_pointer_cast<World>(m_scenes.top())) {
                m_scenes.push(std::make_shared<MenuTransitionIn>());
            }
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

InputWrap & Game::GetInput() { return m_input; }

AssetManager & Game::GetAssetMgr() { return m_assetManager; }

void Game::TryBindGamepad(const sf::Joystick::Identification & ident) {
    auto jsBtnMap = std::find_if(
        m_conf.controls.gamepadMappings.begin(),
        m_conf.controls.gamepadMappings.end(),
        [&ident](const ConfigData::ControlsConf::GamepadMapping & mapping) {
            return mapping.vendorId == ident.vendorId &&
                   mapping.productId == ident.productId;
        });
    if (jsBtnMap != m_conf.controls.gamepadMappings.end()) {
        m_input.joystick = std::unique_ptr<Joystick>(new GamepadJoystick);
        m_input.buttonSet = std::unique_ptr<ButtonSet>(new GamepadButtonSet(*jsBtnMap));
    }
}

void Game::StashWindow() {
    auto windowSize = m_window.getSize();
    std::vector<std::uint8_t> data(windowSize.x * windowSize.y * 4);
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, windowSize.x, windowSize.y, GL_RGBA, GL_UNSIGNED_BYTE,
                 &data[0]);
    m_stash.LoadFromMemory(data, {windowSize.x, windowSize.y});
}

void Game::DisplayStash() {
    auto & txtrdQuadProg =
        m_assetManager.GetProgram<ShaderProgramId::GenericTextured>();
    txtrdQuadProg.Use();
    txtrdQuadProg.SetUniformInt("tex", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_stash.GetId());
    glm::mat4 model;
    model = glm::translate(
        model, {m_window.getSize().x / 2.f, m_window.getSize().y / 2.f, 0.f});
    model = glm::scale(
        model, {m_window.getSize().x / 2.f, m_window.getSize().y / 2.f, 1.f});
    txtrdQuadProg.SetUniformMat4("model", model);
    PRIMITIVES::TexturedQuad quad;
    quad.Display(txtrdQuadProg, {BlendFactor::One, BlendFactor::Zero});
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Game::DrawShadowMap() {
    auto & shadowProgram = m_assetManager.GetProgram<ShaderProgramId::Shadow>();
    shadowProgram.Use();
    glViewport(0, 0, m_window.getSize().x / 2, m_window.getSize().y / 2);
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFB);
    glClear(GL_DEPTH_BUFFER_BIT);
    if (auto plane = m_player.GetPlane()) {
        plane->Display(shadowProgram);
    }
    AssertGLStatus("shadow loop");
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Incomplete framebuffer");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static Game * g_gameRef;

Game::Game(const ConfigData & conf)
    : m_conf(conf),
      m_window(sf::VideoMode::getDesktopMode(),
               conf.localization.strings.appName, sf::Style::Fullscreen,
               sf::ContextSettings(24, 8, conf.graphics.antialiasing, 4, 1,
                                   sf::Style::Default, false)),
      m_running(true), m_planesRegistry(LoadPlanes()), m_seed(time(nullptr)),
      m_restartRequested(false),
      m_terrainManager(std::unique_ptr<TerrainManager>(new MountainousTerrain)) {
    g_gameRef = this;
    glClearColor(0.f, 0.f, 0.f, 1.f);
    m_input.joystick = std::unique_ptr<Joystick>(new MouseJoystickProxy);
    m_input.buttonSet =
        std::unique_ptr<ButtonSet>(new KeyboardButtonSet(m_conf.controls.keyboardMapping));
    auto windowSize = m_window.getSize();
    sf::Mouse::setPosition({static_cast<int>(windowSize.x / 2),
                            static_cast<int>(windowSize.y / 2)});
    m_window.setMouseCursorVisible(!conf.graphics.hideCursor);
    m_window.setVerticalSyncEnabled(conf.graphics.vsyncEnabled);
    SetupGL();
    PRIMITIVES::Init();
    TerrainChunk::InitIndexBufs();
    Text::Enable();
    m_assetManager.LoadResources();
    this->SetupShadowMap();
    Patch::SubvertMacOSKernelPanics(*this);
    Patch::ShadowMapPreliminarySweep(*this);
    m_scenes.push(std::make_shared<CreditsScreen>());
}

PlaneRegistry & Game::GetPlaneRegistry() { return m_planesRegistry; }

void Game::SetupGL() {
#ifdef FLIGHT_WINDOWS
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        throw std::runtime_error("Failed to initialize GLEW");
    }
    const GLubyte * renderer = glGetString(GL_RENDERER);
    const GLubyte * version = glGetString(GL_VERSION);
    std::cout << "Supported renderer: " << renderer << std::endl
              << "Supported OpenGL version: " << version << std::endl;
#endif
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glCullFace(GL_FRONT);
}

void Game::NotifyThreadExceptionOccurred(std::exception_ptr ex) {
    m_threadExceptions.push_back(ex);
}

void Game::Restart() {
    while (!m_scenes.empty()) {
        m_scenes.pop();
    }
    m_seed = time(nullptr);
    m_input.joystick->Zero();
    m_entities.clear();
    m_terrainManager = std::unique_ptr<TerrainManager>(new MountainousTerrain);
    m_scenes.push(std::make_shared<TitleScreen>());
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
                std::lock_guard<std::mutex> lk(this->m_sceneStackMtx);
                currentScene = m_scenes.top();
                this->m_scenes.top()->UpdateState(this->m_scenes);
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
    ThreadGuard logicThreadGrd(&Game::LogicLoop, this);
    try {
        while (m_running) {
            PollEvents();
            std::shared_ptr<Scene> currentScene;
            {
                // NOTE: Because the logic thread might pop a scene
                // while this thread is in the middle of a Scene::Display()
                // call.
                std::lock_guard<std::mutex> lk(m_sceneStackMtx);
                if (m_restartRequested) {
                    Restart();
                    m_restartRequested = false;
                }
                currentScene = m_scenes.top();
            }
            if (currentScene->Display()) {
                m_window.display();
            }
            AssertGLStatus("graphics loop");
            if (!m_threadExceptions.empty()) {
                std::rethrow_exception(m_threadExceptions.front());
            }
        }
    } catch (const std::exception & ex) {
        m_running = false;
        throw std::runtime_error(ex.what());
    }
}

Game::~Game() {
    while (!m_scenes.empty()) {
        m_scenes.pop();
    }
    for (auto ex : m_threadExceptions) {
        try {
            std::rethrow_exception(ex);
        } catch (std::exception & ex) {
            static const std::string errMsg =
                "Unhandled error collected from worker thread: ";
            std::cout << errMsg << ex.what() << std::endl;
        }
    }
}

Game & GetGame() { return *g_gameRef; }

SkyManager & Game::GetSkyMgr() { return m_skyManager; }

TerrainManager & Game::GetTerrainMgr() { return *m_terrainManager.get(); }

CollisionManager & Game::GetCollisionMgr() { return m_collisionManager; }

Camera & Game::GetCamera() {
    if (!m_camera) {
        throw std::runtime_error("Camera not set");
    }
    return *m_camera.get();
}

void Game::SetCamera(std::unique_ptr<Camera> camera) {
    m_camera = std::move(camera);
}

Player & Game::GetPlayer() { return m_player; }

sf::Vector2<unsigned> Game::GetWindowSize() const { return m_window.getSize(); }

GLuint Game::GetShadowMapTxtr() const { return m_shadowMapTxtr; }

bool Game::IsRunning() const { return m_running; }
}
