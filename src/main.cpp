#include <typeinfo>
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
#include <glm/gtc/type_ptr.hpp>
#include "Entity.hpp"
#include "Camera.hpp"
#include "RedTail.hpp"
#include "Player.hpp"

static const int SHADOW_WIDTH = 1024;
static const int SHADOW_HEIGHT = 1024;

class App {
    sf::Window m_window;
    int m_framerate;
    bool m_running;
    Camera m_camera;
    std::chrono::high_resolution_clock::time_point m_deltaPoint;
    Player m_player;
    GLuint m_depthMapFB;
    GLuint m_depthMapTxtr;

    // void SetupDepthMap() {
    // 	glGenFramebuffers(1, &m_depthMapFB);
    // 	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFB);
    // 	glGenTextures(1, &m_depthMapTxtr);
    // 	glBindTexture(GL_TEXTURE_2D, m_depthMapTxtr);
    // 	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
    // 		     SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    // 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // 	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
    // 			       m_depthMapTxtr, 0);
    // 	glDrawBuffer(GL_NONE);
    // 	glReadBuffer(GL_NONE);
    // 	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    // 	    throw std::runtime_error("Unable to set up frame buffer");
    // 	}
    // 	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // }
    
public:
    App(const std::string & name) :
	m_window(sf::VideoMode::getDesktopMode(), name.c_str(), sf::Style::Fullscreen,
		 sf::ContextSettings(24, 8, 4, 4, 1)), m_framerate(80), m_running(true),
	m_player(0) {
        glClearColor(0.0f, 0.42f, 0.70f, 1.f);
	m_window.setMouseCursorVisible(false);
	m_window.setVerticalSyncEnabled(true);
	GetAssets().LoadResources();
	GLuint vao;
        glGenVertexArrays(1, &vao);
	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(vao);
	GLuint shaderProg = GetAssets().GetShaderProgram(ShaderProgramId::Base);
	GLint posLoc = glGetAttribLocation(shaderProg, "position");
	glEnableVertexAttribArray(posLoc);
	GLint texLoc = glGetAttribLocation(shaderProg, "texCoord");
	glEnableVertexAttribArray(texLoc);
 	GLint normLoc = glGetAttribLocation(shaderProg, "normal");
	glEnableVertexAttribArray(normLoc);
	glEnable(GL_DEPTH_TEST);
	const auto windowSize = m_window.getSize();
	const float aspect = static_cast<float>(windowSize.x) /
	    static_cast<float>(windowSize.y);
	glm::mat4 proj = glm::perspective(45.0f,
					  aspect, 0.1f, 100.0f);
	GLint uniProj = glGetUniformLocation(shaderProg, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
	auto startPlane = std::make_shared<RedTail>();
	m_player.GivePlane(startPlane);
	m_camera.SetTarget(startPlane);
	// SetupDepthMap();
    }
    
    int Run() {
	using namespace std::chrono;
	std::thread logicThread([this] {
	    m_deltaPoint = high_resolution_clock::now();
	    while (m_running) {
		const auto deltaTime =
		    duration_cast<microseconds>(high_resolution_clock::now() - m_deltaPoint);
		const auto start = high_resolution_clock::now();
		UpdateLogic(deltaTime.count());
		const auto stop = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>(stop - start);
		static const milliseconds updateCap{1};
		m_deltaPoint = high_resolution_clock::now();
		std::this_thread::sleep_for(updateCap - duration);
	    }
	});
	while (m_running) {
	    const auto start = high_resolution_clock::now();
	    this->PollEvents();
	    this->UpdateGraphics();
	    const auto stop = high_resolution_clock::now();
	    auto duration = duration_cast<microseconds>(stop - start);
	    const auto delayAmount =
		milliseconds(static_cast<int>((1 / static_cast<float>(m_framerate)) * 1000));
	    std::this_thread::sleep_for(delayAmount - duration);
	}
	logicThread.join();
	return EXIT_SUCCESS;
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
	m_player.Update(dt);
	m_camera.Update(dt);
    }

    // void UpdateDepthMap() {
    // 	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    // 	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFB);
    // 	glClearDepth(1.0);
    // 	glClear(GL_DEPTH_BUFFER_BIT);
    //     // TODO: use depth map shader program, setup matrices
    //     // TODO: draw stuff to depth map
    // 	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    // 	    throw std::runtime_error("Incomplete framebuffer");
    // 	}
    // 	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // }
    
    void UpdateGraphics() {
	//UpdateDepthMap();
	const GLuint shaderProg = GetAssets().GetShaderProgram(ShaderProgramId::Base);
	const auto & windowSize = m_window.getSize();
	glViewport(0, 0, windowSize.x, windowSize.y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto view = m_camera.GetView();
	auto invView = glm::inverse(view);
	glm::vec3 eyePos = invView * glm::vec4(0, 0, 0, 1);
	GLint eyePosLoc = glGetUniformLocation(shaderProg, "eyePos");
        glUniform3f(eyePosLoc, eyePos[0], eyePos[1], eyePos[2]);
	assert(glGetError() == GL_NO_ERROR);
	GLint viewLoc = glGetUniformLocation(shaderProg, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	m_player.GetPlane()->Display(shaderProg);
	m_window.display();
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
	    std::cerr << "GL error, code: " << std::hex << err << std::endl;
	    exit(EXIT_FAILURE);
	}
    }
};

int main() {
    App app("game");
    return app.Run();
}
