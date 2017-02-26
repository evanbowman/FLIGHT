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

AssetManager & Game::GetAssets() {
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
    Collision::Context::Init();
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

Game::~Game() {
    Collision::Context::Dispose();
}
    
void Game::Run() {
    ThreadGuard logicThreadGrd([this] {
	sf::Clock clock;
    	while (m_running) {
    	    UpdateCap<1000> cap;
	    auto dt = clock.restart();
	    m_smoothDTProv.Feed(dt.asMicroseconds());
	    m_scenes.top()->UpdateLogic(m_smoothDTProv.Get());
	    m_scenes.top()->UpdateState(m_scenes);
	}
    });
    try {
	while (m_running) {
	    using namespace std::chrono;
	    auto start = high_resolution_clock::now();	    
	    PollEvents();
	    if (m_scenes.top()->Display()) {
		m_window.display();
	    }
	    auto stop = high_resolution_clock::now();
	    auto duration = duration_cast<milliseconds>(stop - start);
	    auto fps = (1.f / duration.count()) * milliseconds(1000).count();
	    std::cout << fps << std::endl;
	    AssertGLStatus("graphics loop");
	}
    } catch (const std::exception & ex) {
	m_running = false;
	throw std::runtime_error(ex.what());
    }
}

Game & GetGame() {
    return *g_gameRef;
}

SkyManager & Game::GetSky() {
    return m_skyManager;
}

TerrainManager & Game::GetTerrain() {
    return m_terrainManager;
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
