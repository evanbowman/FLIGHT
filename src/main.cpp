#include <unordered_map>
#include <string>
#include <OpenGL/gl3.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <thread>
#include <tuple>
#include <stdio.h>
#include <chrono>
#include <algorithm>
#include <array>
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <OpenGL/glu.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Sprite.hpp"
#include "AssetManager.hpp"
#include "MeshBuilder.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "Entity.hpp"
#include "Camera.hpp"
#include "RedTail.hpp"
#include "Player.hpp"
#include "Error.hpp"
#include "TerrainManager.hpp"
#include <noise/noise.h>
#include <noise/noiseutils.h>

GLuint terrainIndices;
GLuint terrainData;
size_t terrainDataSize;
size_t indexCount;

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

class App {
    sf::RenderWindow m_window;
    bool m_running;
    Camera m_camera;
    std::chrono::high_resolution_clock::time_point m_deltaPoint;
    Player m_player;
    GLuint m_shadowMapFB;
    GLuint m_shadowMapTxtr;
    TerrainManager m_terrainManager;
    std::array<int64_t, 100> m_dtSmoothingBuffer;
    enum class State {
	Loading, Running 
    };
    State m_state;
    
    void SetupShadowMap() {
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

    void SetupBaseShader() {
	const GLuint baseProg = GetAssets().GetShaderProgram(ShaderProgramId::Base);
	glUseProgram(baseProg);
	const GLint posLoc = glGetAttribLocation(baseProg, "position");
	glEnableVertexAttribArray(posLoc);
	const GLint texLoc = glGetAttribLocation(baseProg, "texCoord");
	glEnableVertexAttribArray(texLoc);
 	const GLint normLoc = glGetAttribLocation(baseProg, "normal");
	glEnableVertexAttribArray(normLoc);
	AssertGLStatus("base shader setup");
    }

    void SetupShadowShader() {
	const GLuint shadowProg = GetAssets().GetShaderProgram(ShaderProgramId::Shadow);
	glUseProgram(shadowProg);
	const GLint posLoc = glGetAttribLocation(shadowProg, "position");
	glEnableVertexAttribArray(posLoc);
	const GLint projLoc = glGetUniformLocation(shadowProg, "proj");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(::LIGHT_PROJ_MAT));
	AssertGLStatus("shadow shader setup");
    }

    void SetupTerrainShader() {
	const GLuint terrainProg = GetAssets().GetShaderProgram(ShaderProgramId::Terrain);
	glUseProgram(terrainProg);
	const GLint posLoc = glGetAttribLocation(terrainProg, "position");
	glEnableVertexAttribArray(posLoc);
 	const GLint normLoc = glGetAttribLocation(terrainProg, "normal");
	glEnableVertexAttribArray(normLoc);
    }
    
    void SetupShaders() {
	this->SetupShadowShader();
	this->SetupTerrainShader();
	this->SetupBaseShader();
    }
    
    void PollEvents() {
	sf::Event event;
	while (m_window.pollEvent(event)) {
	    switch (event.type) {
	    case sf::Event::Closed:
		m_running = false;
		break;

	    case sf::Event::GainedFocus:
		// TODO
		break;

	    case sf::Event::LostFocus:
		// TODO
	        break;
	    }
	}
    }

    void UpdateLogic(const long long dt) {
	switch (m_state) {
	case State::Loading: {
	    m_camera.Update(dt);
	    const auto view = m_camera.GetCameraView();
	    auto invView = glm::inverse(view);
	    glm::vec3 eyePos = invView * glm::vec4(0, 0, 0, 1);
	    m_terrainManager.UpdateChunkLOD(eyePos, m_camera.GetViewDir());
	    if (!m_terrainManager.IsLoadingChunks()) {
		m_state = State::Running;
	    }
	}break;

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

    void DrawShadowMap() {
	const GLuint shadowProgram = GetAssets().GetShaderProgram(ShaderProgramId::Shadow);
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

    void UpdateProjections() {
	const GLuint shadowProgram = GetAssets().GetShaderProgram(ShaderProgramId::Shadow);
	const GLuint lightingProg = GetAssets().GetShaderProgram(ShaderProgramId::Base);
	const GLuint terrainProg = GetAssets().GetShaderProgram(ShaderProgramId::Terrain);
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

    void DrawTerrain() {
	const GLuint terrainProg = GetAssets().GetShaderProgram(ShaderProgramId::Terrain);
	glUseProgram(terrainProg);
	const auto view = m_camera.GetCameraView();
	auto invView = glm::inverse(view);
	glm::vec3 eyePos = invView * glm::vec4(0, 0, 0, 1);
	const GLint eyePosLoc = glGetUniformLocation(terrainProg, "eyePos");
	glUniform3f(eyePosLoc, eyePos[0], eyePos[1], eyePos[2]);
	m_terrainManager.Display(terrainProg);
	AssertGLStatus("terrain rendering");
    }
    
    void UpdateGraphics() {
	switch (m_state) {
	case State::Loading:
	    m_terrainManager.SwapChunks();
	    break;
	    
	case State::Running: {
	    UpdateProjections();
	    m_terrainManager.SwapChunks();
	    this->DrawShadowMap();
	    const auto & windowSize = m_window.getSize();
	    glViewport(0, 0, windowSize.x, windowSize.y);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    this->DrawTerrain();
	    const GLuint lightingProg = GetAssets().GetShaderProgram(ShaderProgramId::Base);
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
	    m_window.display();
	} break;
	}
	AssertGLStatus("graphics loop");
    }

    int64_t SmoothDT(const int64_t currentDT) {
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
    
public:
    App(const std::string & name) :
	m_window(sf::VideoMode::getDesktopMode(), name.c_str(), sf::Style::Fullscreen,
		 sf::ContextSettings(24, 8, 4, 4, 1)), m_running(true),
	m_player(0), m_dtSmoothingBuffer{} {
        glClearColor(0.f, 0.42f, 0.70f, 1.f);
	m_window.setMouseCursorVisible(false);
	m_window.setVerticalSyncEnabled(true);
	GetAssets().LoadResources();
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
    
    void Run() {
	using namespace std::chrono;
	sf::Clock clock;
	std::thread logicThread([&clock, this] {
	    while (m_running) {
		auto dt = clock.restart();
		UpdateLogic(SmoothDT(dt.asMicroseconds()));
	    }
	});
	std::thread terrainThread([this] {
	    // Generating terrain from fractal noise is computationally intensive
	    // enough that it really does need it's own thread
	    while (this->m_running) {
		if (m_state == State::Loading || m_state == State::Running) {
		    auto start = high_resolution_clock::now();
		    this->m_terrainManager.UpdateTerrainGen();
		    auto stop = high_resolution_clock::now();
		    auto duration = duration_cast<microseconds>(stop - start);
		    static const milliseconds updateCap{1};
		    std::this_thread::sleep_for(updateCap - duration);
		}
	    }
	});
	try {
	    while (m_running) {
		this->PollEvents();
		this->UpdateGraphics();
	    }
	} catch (const std::exception & ex) {
	    m_running = false;
	    logicThread.join();
	    throw std::runtime_error(ex.what());
	}
	logicThread.join();
	terrainThread.join();
    }
};

int main() {
    try {
	App app("game");
	app.Run();
    } catch (const std::exception & ex) {
	std::cerr << ex.what() << std::endl;
    }
}
