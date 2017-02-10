#include "App.hpp"

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

void App::SetupShadowMap() {
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

void App::SetupBaseShader() {
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

void App::SetupShadowShader() {
    const GLuint shadowProg = m_assetManager.GetShaderProgram(ShaderProgramId::Shadow);
    glUseProgram(shadowProg);
    const GLint posLoc = glGetAttribLocation(shadowProg, "position");
    glEnableVertexAttribArray(posLoc);
    const GLint projLoc = glGetUniformLocation(shadowProg, "proj");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(::LIGHT_PROJ_MAT));
    AssertGLStatus("shadow shader setup");
}

void App::SetupTerrainShader() {
    const GLuint terrainProg = m_assetManager.GetShaderProgram(ShaderProgramId::Terrain);
    glUseProgram(terrainProg);
    const GLint posLoc = glGetAttribLocation(terrainProg, "position");
    glEnableVertexAttribArray(posLoc);
    const GLint normLoc = glGetAttribLocation(terrainProg, "normal");
    glEnableVertexAttribArray(normLoc);
}
    
void App::SetupShaders() {
    this->SetupShadowShader();
    this->SetupTerrainShader();
    this->SetupBaseShader();
}
    
void App::PollEvents() {
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

InputWrap & App::GetInput() {
    return m_input;
}

void App::UpdateLogic(const long long dt) {
    switch (m_state) {
    case State::Loading: {
	m_camera.Update(dt);
	const auto view = m_camera.GetCameraView();
	auto invView = glm::inverse(view);
	glm::vec3 eyePos = invView * glm::vec4(0, 0, 0, 1);
	m_terrainManager.UpdateChunkLOD(eyePos, m_camera.GetViewDir());
	if (!m_terrainManager.HasWork()) {
	    m_state = State::Running;
	}
    } break;

    case State::Running: {
	m_player.Update(dt);
	m_camera.Update(dt);
	const auto view = m_camera.GetCameraView();
	auto invView = glm::inverse(view);
	glm::vec3 eyePos = invView * glm::vec4(0, 0, 0, 1);
	m_terrainManager.UpdateChunkLOD(eyePos, m_camera.GetViewDir());
    } break;
    }
}

AssetManager & App::GetAssets() {
    return m_assetManager;
}

void App::DrawShadowMap() {
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

void App::UpdateProjectionUniforms() {
    const GLuint shadowProgram = m_assetManager.GetShaderProgram(ShaderProgramId::Shadow);
    const GLuint lightingProg = m_assetManager.GetShaderProgram(ShaderProgramId::Base);
    const GLuint terrainProg = m_assetManager.GetShaderProgram(ShaderProgramId::Terrain);
    glUseProgram(shadowProgram);
    auto view = m_camera.GetLightView();
    auto lightSpace = ::LIGHT_PROJ_MAT * view;
    GLint lightSpaceLoc = glGetUniformLocation(shadowProgram, "lightSpace");
    glUniformMatrix4fv(lightSpaceLoc, 1, GL_FALSE, glm::value_ptr(lightSpace));

    glUseProgram(lightingProg);
    view = m_camera.GetCameraView();
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
    
void App::DrawTerrain() {
    const GLuint terrainProg = m_assetManager.GetShaderProgram(ShaderProgramId::Terrain);
    glUseProgram(terrainProg);
    const auto view = m_camera.GetCameraView();
    auto invView = glm::inverse(view);
    glm::vec3 eyePos = invView * glm::vec4(0, 0, 0, 1);
    const GLint eyePosLoc = glGetUniformLocation(terrainProg, "eyePos");
    glUniform3f(eyePosLoc, eyePos[0], eyePos[1], eyePos[2]);
    m_terrainManager.Display(terrainProg);
    AssertGLStatus("terrain rendering");
}
    
void App::UpdateGraphics() {
    switch (m_state) {
    case State::Loading:
	m_terrainManager.SwapChunks();
	break;
	    
    case State::Running:
	{
	    std::lock_guard<std::mutex> lk(m_logicMutex);
	    this->UpdateProjectionUniforms();
	    m_terrainManager.SwapChunks();
	    this->DrawShadowMap();
	    const auto & windowSize = m_window.getSize();
	    glViewport(0, 0, windowSize.x, windowSize.y);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    this->DrawTerrain();
	    const GLuint lightingProg = m_assetManager.GetShaderProgram(ShaderProgramId::Base);
	    glUseProgram(lightingProg);
	    const auto view = m_camera.GetCameraView();
	    auto invView = glm::inverse(view);
	    glm::vec3 eyePos = invView * glm::vec4(0, 0, 0, 1);
	    const GLint eyePosLoc = glGetUniformLocation(lightingProg, "eyePos");
	    glUniform3f(eyePosLoc, eyePos[0], eyePos[1], eyePos[2]);
	    glUniform1i(glGetUniformLocation(lightingProg, "shadowMap"), 1);
	    glActiveTexture(GL_TEXTURE1);
	    glBindTexture(GL_TEXTURE_2D, m_shadowMapTxtr);
	    m_player.GetPlane()->Display(lightingProg);
	}
	m_window.display();
	break;
    }
    AssertGLStatus("graphics loop");
}

int64_t App::SmoothDT(const int64_t currentDT) {
    for (size_t i = 1; i < m_dtSmoothingBuffer.size(); ++i) {
	m_dtSmoothingBuffer[i] = m_dtSmoothingBuffer[i - 1];
    }
    m_dtSmoothingBuffer[0] = currentDT;
    int64_t sum = 0;
    for (auto dt : m_dtSmoothingBuffer) {
	sum += dt;
    }
    return sum / m_dtSmoothingBuffer.size();
}

static App * g_appRef;

App::App(const std::string & name) :
    m_window(sf::VideoMode::getDesktopMode(), name.c_str(), sf::Style::Fullscreen,
	     sf::ContextSettings(24, 8, 4, 4, 1)), m_running(true),
    m_player(0), m_dtSmoothingBuffer{} {
    g_appRef = this;
    glClearColor(0.f, 0.42f, 0.70f, 1.f);
    // Use the mouse as the default input mode for now
    m_input.joystick = std::make_unique<MouseProxy>();
    auto windowSize = m_window.getSize();
    sf::Mouse::setPosition({static_cast<int>(windowSize.x / 2),
			    static_cast<int>(windowSize.y / 2)});
    m_window.setMouseCursorVisible(false);
    m_window.setVerticalSyncEnabled(true);
    m_assetManager.LoadResources();
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(vao);
    this->SetupShaders();
    auto startPlane = std::make_shared<RedTail>();
    startPlane->SetPosition({15.f, 30.f, 15.f});
    m_player.GivePlane(startPlane);
    m_camera.SetTarget(startPlane);
    GLint drawFboId, readFboId;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFboId);
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readFboId);
    this->SetupShadowMap();
    patch::SubvertMacOSKernelPanics(m_window);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
}
    
void App::Run() {
    sf::Clock clock;
    std::mutex mtx;
    ThreadGuard logicThreadGrd([&mtx, &clock, this] {
    	while (m_running) {
    	    UpdateCap<1000> cap;
	    auto dt = clock.restart();
	    {
		std::lock_guard<std::mutex> lk(m_logicMutex);
		UpdateLogic(SmoothDT(dt.asMicroseconds()));
	    }
	}
    });
    ThreadGuard terrainGenThread1Grd([this] {
    	// Generating terrain from fractal noise is computationally intensive
    	// enough that it really does need it's own thread; it would choke up
    	// the logic thread.
    	while (m_running) {
    	    if (m_state == State::Loading || m_state == State::Running) {
    		if (m_terrainManager.HasWork()) {
    		    m_terrainManager.UpdateTerrainGen();
    		} else {
    		    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    		}
    	    }
    	}
    });
    try {
	while (m_running) {
	    using namespace std::chrono;
	    auto start = high_resolution_clock::now();
	    this->PollEvents();
	    this->UpdateGraphics();
	    auto stop = high_resolution_clock::now();
	    auto duration = duration_cast<milliseconds>(stop - start);
	    auto fps = (1.f / duration.count()) * milliseconds(1000).count();
	    //std::cout << fps << std::endl;
	}
    } catch (const std::exception & ex) {
	m_running = false;
	throw std::runtime_error(ex.what());
    }
}

App & GetApp() {
    return *g_appRef;
}
