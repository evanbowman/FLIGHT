#include "Game.hpp"

static const glm::mat4 LIGHT_PROJ_MAT = glm::ortho(-4.f, 4.f, -4.f, 4.f, -5.f, 12.f);	

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

void Game::SetupSkyShader() {}

void Game::SetupBaseShader() {
    const GLuint baseProg = m_assetManager.GetShaderProgram(ShaderProgramId::Base);
    glUseProgram(baseProg);
    const GLint posLoc = glGetAttribLocation(baseProg, "position");
    glEnableVertexAttribArray(posLoc);
    const GLint texLoc = glGetAttribLocation(baseProg, "texCoord");
    glEnableVertexAttribArray(texLoc);
    const GLint normLoc = glGetAttribLocation(baseProg, "normal");
    glEnableVertexAttribArray(normLoc);
    AssertGLStatus("base shader setup");
}

void Game::SetupShadowShader() {
    const GLuint shadowProg = m_assetManager.GetShaderProgram(ShaderProgramId::Shadow);
    glUseProgram(shadowProg);
    const GLint posLoc = glGetAttribLocation(shadowProg, "position");
    glEnableVertexAttribArray(posLoc);
    const GLint projLoc = glGetUniformLocation(shadowProg, "proj");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(::LIGHT_PROJ_MAT));
    AssertGLStatus("shadow shader setup");
}

void Game::SetupTerrainShader() {
    const GLuint terrainProg = m_assetManager.GetShaderProgram(ShaderProgramId::Terrain);
    glUseProgram(terrainProg);
    const GLint posLoc = glGetAttribLocation(terrainProg, "position");
    glEnableVertexAttribArray(posLoc);
    const GLint normLoc = glGetAttribLocation(terrainProg, "normal");
    glEnableVertexAttribArray(normLoc);
}
    
void Game::SetupShaders() {
    this->SetupShadowShader();
    this->SetupTerrainShader();
    this->SetupBaseShader();
}
    
void Game::PollEvents() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
	switch (event.type) {
	case sf::Event::Closed:
	    m_running = false;
	    break;

	case sf::Event::MouseMoved:
	    if (auto mouseProxy = dynamic_cast<MouseProxy *>(m_input.joystick.get())) {
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
		    m_input.joystick = std::make_unique<MouseProxy>();
		}
	    }
	    break;
	    
	case sf::Event::GainedFocus:
	    if (dynamic_cast<MouseProxy *>(m_input.joystick.get())) {
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

void Game::UpdateProjectionUniforms() {
    const GLuint shadowProgram = m_assetManager.GetShaderProgram(ShaderProgramId::Shadow);
    const GLuint lightingProg = m_assetManager.GetShaderProgram(ShaderProgramId::Base);
    const GLuint terrainProg = m_assetManager.GetShaderProgram(ShaderProgramId::Terrain);
    glUseProgram(shadowProgram);
    auto view = m_camera.GetLightView();
    auto lightSpace = ::LIGHT_PROJ_MAT * view;
    GLint lightSpaceLoc = glGetUniformLocation(shadowProgram, "lightSpace");
    glUniformMatrix4fv(lightSpaceLoc, 1, GL_FALSE, glm::value_ptr(lightSpace));

    glUseProgram(lightingProg);
    view = m_camera.GetWorldView();
    const auto & windowSize = m_window.getSize();
    const float aspect = static_cast<float>(windowSize.x) /
	static_cast<float>(windowSize.y);
    const glm::mat4 perspective = glm::perspective(45.0f, aspect, 0.1f, 1.0f);
    auto cameraSpace = perspective * view;
    GLint cameraSpaceLoc = glGetUniformLocation(lightingProg, "cameraSpace");
    lightSpaceLoc = glGetUniformLocation(lightingProg, "lightSpace");
    glUniformMatrix4fv(cameraSpaceLoc, 1, GL_FALSE, glm::value_ptr(cameraSpace));
    glUniformMatrix4fv(lightSpaceLoc, 1, GL_FALSE, glm::value_ptr(lightSpace));

    glUseProgram(terrainProg);
    cameraSpaceLoc = glGetUniformLocation(terrainProg, "cameraSpace");
    glUniformMatrix4fv(cameraSpaceLoc, 1, GL_FALSE, glm::value_ptr(cameraSpace));
}
    
void Game::DrawTerrain() {
    const GLuint terrainProg = m_assetManager.GetShaderProgram(ShaderProgramId::Terrain);
    glUseProgram(terrainProg);
    const auto view = m_camera.GetWorldView();
    auto invView = glm::inverse(view);
    glm::vec3 eyePos = invView * glm::vec4(0, 0, 0, 1);
    const GLint eyePosLoc = glGetUniformLocation(terrainProg, "eyePos");
    glUniform3f(eyePosLoc, eyePos[0], eyePos[1], eyePos[2]);
    m_terrainManager.Display(terrainProg);
    AssertGLStatus("terrain rendering");
}

int64_t Game::SmoothDT(const int64_t currentDT) {
    m_dtSmoothingBuffer[m_dtSmoothingTracker++ % m_dtSmoothingBuffer.size()] = currentDT;
    int64_t aggregate = 0;
    for (auto dt : m_dtSmoothingBuffer) {
	aggregate += dt;
    }
    return aggregate / m_dtSmoothingBuffer.size();
}

static Game * g_gameRef;

Game::Game(const std::string & name) :
    m_window(sf::VideoMode::getDesktopMode(), name.c_str(), sf::Style::Fullscreen,
	     sf::ContextSettings(24, 8, 4, 4, 1)), m_running(true),
    m_player(0), m_dtSmoothingBuffer{}, m_dtSmoothingTracker(0), m_state(State::Loading) {
    g_gameRef = this;
    glClearColor(0.1f, 0.52f, 0.80f, 1.f);
    m_input.joystick = std::make_unique<MouseProxy>();
    auto windowSize = m_window.getSize();
    sf::Mouse::setPosition({static_cast<int>(windowSize.x / 2),
			    static_cast<int>(windowSize.y / 2)});
    m_window.setMouseCursorVisible(false);
    m_window.setVerticalSyncEnabled(true);
    m_assetManager.LoadResources();
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnable(GL_DEPTH_TEST);
    this->SetupShaders();
    this->SetupShadowMap();
    patch::SubvertMacOSKernelPanics(m_window);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    m_scenes.push(std::make_unique<WorldLoader>());
}
    
void Game::Run() {
    ThreadGuard logicThreadGrd([this] {
	sf::Clock clock;
    	while (m_running) {
    	    UpdateCap<1000> cap;
	    auto dt = clock.restart();
	    m_scenes.top()->Update(SmoothDT(dt.asMicroseconds()));
	}

    });
    try {
	while (m_running) {
	    PollEvents();
	    m_scenes.top()->Display();
	    m_window.display();
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

TerrainManager & Game::GetTerrain() {
    return m_terrainManager;
}

Camera & Game::GetCamera() {
    return m_camera;
}

Player & Game::GetPlayer() {
    return m_player;
}

void Game::PushScene(std::unique_ptr<Scene> scene) {
    m_scenes.push(std::move(scene));
}

Game::~Game() {
    
}

sf::Vector2<unsigned> Game::GetWindowSize() const {
    return m_window.getSize();
}

GLuint Game::GetShadowMapTxtr() const {
    return m_shadowMapTxtr;
}
