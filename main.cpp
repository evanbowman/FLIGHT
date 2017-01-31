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

class App {
    sf::Window m_window;
    int m_framerate;

public:
    App(const std::string & name) :
	m_window(sf::VideoMode::getDesktopMode(), name.c_str(), sf::Style::Fullscreen,
		 sf::ContextSettings(24, 8, 4, 4, 1
				     )), m_framerate(120) {
        glClearColor(0.03f, 0.41f, 0.58f, 1.f);
	m_window.setMouseCursorVisible(false);
	GetAssets().LoadResources();
	GLuint vao;
        glGenVertexArrays(1, &vao);
	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(vao);
    }
    
    int Run() {
	bool running = true;
	const float aspect = static_cast<float>(m_window.getSize().x) /
	    static_cast<float>(m_window.getSize().y);
	GLuint shaderProg = GetAssets().GetShaderProgram(ShaderProgramId::Base);
	glm::mat4 proj = glm::perspective(45.0f,
					  aspect, 0.1f, 100.0f);
	GLint uniProj = glGetUniformLocation(shaderProg, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
	glm::mat4 view;
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0f, 0.f, 1.0f));
	while (running) {
	    using namespace std::chrono;
	    const auto start = high_resolution_clock::now();
	    sf::Event event;
	    while (m_window.pollEvent(event)) {
		switch (event.type) {
		case sf::Event::Closed:
		    running = false;
		    break;
		}
	    }
	    //view = glm::rotate(view, 0.005f, glm::vec3(1, 1 ,1));
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    auto vbo = GetAssets().GetModel(ModelId::Wing)->m_vbo;
	    auto bufferSize = GetAssets().GetModel(ModelId::Wing)->m_vertCoordSize;
	    GLint uniView = glGetUniformLocation(shaderProg, "view");
	    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
	    glBindBuffer(GL_ARRAY_BUFFER, vbo);
	    GLint posAttrib = glGetAttribLocation(shaderProg, "position");
	    glEnableVertexAttribArray(posAttrib);
	    GLint texAttrib = glGetAttribLocation(shaderProg, "texcoord");
	    glEnableVertexAttribArray(texAttrib);
	    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
	      			  (void *)(2 * sizeof(glm::vec3)));
	    model = glm::rotate(model, 0.005f, glm::vec3(1, 1, 1));
	    GLint uniModel = glGetUniformLocation(shaderProg, "model");
	    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
	    glBindTexture(GL_TEXTURE_2D, GetAssets().GetTexture(TextureId::Wing)->GetId());
	    glUniform1i(glGetUniformLocation(shaderProg, "tex"), 0);
	    glDrawArrays(GL_TRIANGLES, 0, bufferSize);
	    m_window.display();
	    GLenum err = glGetError();
	    if (err != GL_NO_ERROR) {
		std::cerr << "GL error, code: " << err << std::endl;
		std::cerr << gluErrorString(err) << std::endl;
		return EXIT_FAILURE;
	    }
	    const auto stop = high_resolution_clock::now();
	    auto duration = duration_cast<microseconds>(stop - start);
	    const auto delayAmount =
		milliseconds(static_cast<int>((1 / static_cast<float>(m_framerate)) * 1000));
	    std::this_thread::sleep_for(delayAmount - duration);
	}
	return EXIT_SUCCESS;
    }
};

int main() {
    App app("game");
    return app.Run();
}
