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
#include "RedTail.hpp"

class App {
    sf::Window m_window;
    int m_framerate;
    bool m_running;
    std::chrono::high_resolution_clock::time_point m_deltaPoint;
    std::shared_ptr<Entity> m_playerPlane;

public:
    App(const std::string & name) :
	m_window(sf::VideoMode::getDesktopMode(), name.c_str(), sf::Style::Fullscreen,
		 sf::ContextSettings(24, 8, 4, 4, 1)), m_framerate(120), m_running(true) {
        glClearColor(0.03f, 0.41f, 0.58f, 1.f);
	m_window.setMouseCursorVisible(false);
	GetAssets().LoadResources();
	GLuint vao;
        glGenVertexArrays(1, &vao);
	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(vao);
	GLuint shaderProg = GetAssets().GetShaderProgram(ShaderProgramId::Base);
	GLint posAttrib = glGetAttribLocation(shaderProg, "position");
	glEnableVertexAttribArray(posAttrib);
	GLint texAttrib = glGetAttribLocation(shaderProg, "texcoord");
	glEnableVertexAttribArray(texAttrib);
	glEnable(GL_DEPTH_TEST);
	const float aspect = static_cast<float>(m_window.getSize().x) /
	    static_cast<float>(m_window.getSize().y);
	glm::mat4 proj = glm::perspective(45.0f,
					  aspect, 0.1f, 100.0f);
	GLint uniProj = glGetUniformLocation(shaderProg, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
	m_playerPlane = std::make_shared<RedTail>();
    }
    
    int Run() {
	using namespace std::chrono;
	std::thread logicThread([this] {
	    while (m_running) {
		const auto deltaTime =
		    duration_cast<microseconds>(high_resolution_clock::now() - m_deltaPoint);
		const auto start = high_resolution_clock::now();
		UpdateLogic();
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
	    }
	}
    }

    void UpdateLogic() {
	// ...
    }
    
    void UpdateGraphics() {
	const GLuint shaderProg = GetAssets().GetShaderProgram(ShaderProgramId::Base);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 view;
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	// TODO: Update view...
	GLint uniView = glGetUniformLocation(shaderProg, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
	m_playerPlane->Display(shaderProg);
	m_window.display();
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
	    std::cerr << "GL error, code: " << err << std::endl;
	    std::cerr << gluErrorString(err) << std::endl;
	    exit(EXIT_FAILURE);
	}
    }
};

int main() {
    App app("game");
    return app.Run();
}
