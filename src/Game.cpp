#include "Game.hpp"

namespace patch {
    // I was getting kernel panics on macOS when trying to draw
    // to an offscreen framebuffer before having displayed the window
    // at least once.
    void SubvertMacOSKernelPanics(sf::Window & window) {
	sf::Event event;
	while (window.pollEvent(event));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	window.display();
    }
}

static const int SHADOW_WIDTH = 1400;
static const int SHADOW_HEIGHT = 1400;

void Game::SetupShadowMap() {
    glGenFramebuffers(1, &m_shadowMapFB);
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFB);
    glGenTextures(1, &m_shadowMapTxtr);
    glBindTexture(GL_TEXTURE_2D, m_shadowMapTxtr);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
		 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
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

	case sf::Event::MouseMoved:
	    if (auto mouseProxy =
		dynamic_cast<MouseJoystickProxy *>(m_input.joystick.get())) {
		if (!mouseProxy->Yield()) {
		    mouseProxy->Update(event.mouseMove);
		}
	    }
	    break;

	case sf::Event::JoystickMoved:
	    if (auto js = dynamic_cast<Joystick *>(m_input.joystick.get())) {
		js->Update();
	    }
	    break;

	case sf::Event::JoystickConnected:
	    if (event.joystickConnect.joystickId == 0) {
		m_input.joystick = std::make_unique<Joystick>();
	    }
	    break;

	case sf::Event::JoystickDisconnected:
	    if (event.joystickConnect.joystickId == 0) {
		if (dynamic_cast<Joystick *>(m_input.joystick.get())) {
		    m_input.joystick = std::make_unique<MouseJoystickProxy>();
		}
	    }
	    break;
	    
	case sf::Event::GainedFocus:
	    if (dynamic_cast<MouseJoystickProxy *>(m_input.joystick.get())) {
		auto windowSize = m_window.getSize();
		sf::Mouse::setPosition({static_cast<int>(windowSize.x / 2),
					static_cast<int>(windowSize.y / 2)});
	    }
	    break;

	case sf::Event::LostFocus:
	    // TODO
	    break;
	}
    }
}

InputWrap & Game::GetInput() {
    return m_input;
}

AssetManager & Game::GetAssetMgr() {
    return m_assetManager;
}

void Game::DrawShadowMap() {
    const GLuint shadowProgram = m_assetManager.GetShaderProgram(ShaderProgramId::Shadow);
    glUseProgram(shadowProgram);
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

Game::Game(const std::string & name) :
    m_window(sf::VideoMode::getDesktopMode(), name.c_str(), sf::Style::Fullscreen,
	     sf::ContextSettings(24, 8, 4, 4, 1)), m_running(true), m_player(0) {
    g_gameRef = this;
    glClearColor(0.f, 0.f, 0.f, 1.f);
    m_input.joystick = std::make_unique<MouseJoystickProxy>();
    auto windowSize = m_window.getSize();
    sf::Mouse::setPosition({static_cast<int>(windowSize.x / 2),
			    static_cast<int>(windowSize.y / 2)});
    m_window.setMouseCursorVisible(false);
    m_window.setVerticalSyncEnabled(true);
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    Primitives::Init();
    FontFace::Init();
    Text::Init();
    m_assetManager.LoadResources();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glCullFace(GL_FRONT);
    this->SetupShadowMap();
    patch::SubvertMacOSKernelPanics(m_window);
    m_scenes.push(std::make_unique<TitleScreen>());
}

void Game::NotifyThreadExceptionOccurred(std::exception_ptr ex) {
    m_threadExceptions.push_back(ex);
}

void Game::Run() {
    ThreadGuard logicThreadGrd([this] {
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
	    }
	} catch (const std::exception & ex) {
	    this->NotifyThreadExceptionOccurred(std::current_exception());
	}
    });
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

Game & GetGame() {
    return *g_gameRef;
}

SkyManager & Game::GetSkyMgr() {
    return m_skyManager;
}

TerrainManager & Game::GetTerrainMgr() {
    return m_terrainManager;
}

CollisionManager & Game::GetCollisionMgr() {
    return m_collisionManager;
}

Camera & Game::GetCamera() {
    return m_camera;
}

Player & Game::GetPlayer() {
    return m_player;
}

sf::Vector2<unsigned> Game::GetWindowSize() const {
    return m_window.getSize();
}

GLuint Game::GetShadowMapTxtr() const {
    return m_shadowMapTxtr;
}

bool Game::IsRunning() const {
    return m_running;
}
