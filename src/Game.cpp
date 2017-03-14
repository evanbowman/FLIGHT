#include "Game.hpp"

namespace FLIGHT {
namespace patch {
// I was getting kernel panics on macOS when trying to draw
// to an offscreen framebuffer before having displayed the window
// at least once.
void SubvertMacOSKernelPanics(sf::Window & window) {
    sf::Event event;
    while (window.pollEvent(event))
        ;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    window.display();
}
}

static const int SHADOW_WIDTH = 1400;
static const int SHADOW_HEIGHT = 1400;

void Game::SetSeed(const time_t seed) { m_seed = seed; }

time_t Game::GetSeed() const { return m_seed; }

void Game::UpdateEntities(const Time dt) {
    {
        std::lock_guard<std::mutex> lk(m_entitiesMtx);
        auto front = m_entities.begin();
        if (front != m_entities.end()) {
            if ((*front)->GetDeallocFlag()) {
                m_entities.erase_after(m_entities.before_begin());
            }
        }
    }
    for (auto it = m_entities.begin(); it != m_entities.end();) {
        auto current = it;
        (*current)->Update(dt);
        auto next = std::next(it);
        if (next != m_entities.end()) {
            if ((*next)->GetDeallocFlag()) {
                current = m_entities.erase_after(current);
            }
        }
        it = std::next(current);
    }
}

void Game::SetupShadowMap() {
    glGenFramebuffers(1, &m_shadowMapFB);
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFB);
    glGenTextures(1, &m_shadowMapTxtr);
    glBindTexture(GL_TEXTURE_2D, m_shadowMapTxtr);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH,
                 SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
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
                m_input.joystick = std::make_unique<MouseJoystickProxy>();
                m_input.buttonSet = std::make_unique<KeyboardButtonSet>(
                    m_conf.controls.keyboardMapping);
            }
            break;

        case sf::Event::GainedFocus:
            m_focused = true;
            break;

        case sf::Event::LostFocus:
            m_focused = false;
            break;
        }
    }
}

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
        m_input.joystick = std::make_unique<GamepadJoystick>();
        m_input.buttonSet = std::make_unique<GamepadButtonSet>(*jsBtnMap);
    }
}

void Game::DrawShadowMap() {
    auto & shadowProgram = m_assetManager.GetProgram<ShaderProgramId::Shadow>();
    shadowProgram.Use();
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFB);
    glClear(GL_DEPTH_BUFFER_BIT);
    m_player.GetPlane()->Display(shadowProgram);
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
      m_running(true), m_focused(false), m_planesRegistry(LoadPlanes()),
      m_seed(time(nullptr)) {
    g_gameRef = this;
    glClearColor(0.f, 0.f, 0.f, 1.f);
    m_input.joystick = std::make_unique<MouseJoystickProxy>();
    m_input.buttonSet =
        std::make_unique<KeyboardButtonSet>(m_conf.controls.keyboardMapping);
    auto windowSize = m_window.getSize();
    sf::Mouse::setPosition({static_cast<int>(windowSize.x / 2),
                            static_cast<int>(windowSize.y / 2)});
    m_window.setMouseCursorVisible(!conf.graphics.hideCursor);
    m_window.setVerticalSyncEnabled(conf.graphics.vsyncEnabled);
    SetupGL();
    PRIMITIVES::Init();
    m_window.requestFocus();
    m_assetManager.LoadResources();
    this->SetupShadowMap();
    patch::SubvertMacOSKernelPanics(m_window);
    m_scenes.push(std::make_unique<TitleScreen>());
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

void Game::LogicLoop() {
    sf::Clock clock;
    try {
        while (m_running) {
            UpdateCap<1000> cap;
            auto dt = clock.restart();
            this->m_smoothDTProv.Feed(dt.asMicroseconds());
            this->m_scenes.top()->UpdateLogic(m_smoothDTProv.Get());
            {
                std::lock_guard<std::mutex> lk(this->m_sceneStackMtx);
                this->m_scenes.top()->UpdateState(this->m_scenes);
            }
            if (GAMEFEEL::WasPaused()) {
                clock.restart();
                GAMEFEEL::Reset();
            }
        }
    } catch (const std::exception &) {
        this->NotifyThreadExceptionOccurred(std::current_exception());
    }
}

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

TerrainManager & Game::GetTerrainMgr() { return m_terrainManager; }

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
