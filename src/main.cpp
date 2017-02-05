
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
#include "MeshBuilder.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "Entity.hpp"
#include "Camera.hpp"
#include "RedTail.hpp"
#include "Player.hpp"
#include "Error.hpp"
#include "MapChunk.hpp"
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
    int m_framerate;
    bool m_running;
    Camera m_camera;
    std::chrono::high_resolution_clock::time_point m_deltaPoint;
    Player m_player;
    GLuint m_shadowMapFB;
    GLuint m_shadowMapTxtr;

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
	m_player.Update(dt);
	m_camera.Update(dt);
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
	const glm::mat4 perspective = glm::perspective(45.0f, aspect, 0.1f, 100.0f);
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
	const GLint modelLoc = glGetUniformLocation(terrainProg, "model");
	glm::mat4 model;
	model = glm::translate(model, {0, 0, 0});
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	const GLint invTransModelLoc = glGetUniformLocation(terrainProg, "invTransModel");
	glm::mat4 invTransModel = glm::transpose(glm::inverse(model));
	glUniformMatrix4fv(invTransModelLoc, 1, GL_FALSE, glm::value_ptr(invTransModel));
	glBindBuffer(GL_ARRAY_BUFFER, terrainData);
       	GLint posAttrib = glGetAttribLocation(terrainProg, "position");
	GLint normAttrib = glGetAttribLocation(terrainProg, "normal");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVert), 0);
	glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVert),
			      (void *)sizeof(glm::vec3));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainIndices);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, 0);
	AssertGLStatus("terrain rendering");
    }
    
    void UpdateGraphics() {
	UpdateProjections();
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
	AssertGLStatus("graphics loop");
    }
    
public:
    App(const std::string & name) :
	m_window(sf::VideoMode::getDesktopMode(), name.c_str(), sf::Style::Fullscreen,
		 sf::ContextSettings(24, 8, 4, 4, 1)), m_framerate(80), m_running(true),
	m_player(0) {
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
	m_player.GivePlane(startPlane);
	m_camera.SetTarget(startPlane);
	GLint drawFboId, readFboId;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFboId);
	glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readFboId);
	this->SetupShadowMap();
	patch::SubvertMacOSKernelPanics(m_window);
	TestTerrain();
    }

    void TestTerrain() {
	module::RidgedMulti mountainTerrain;
	mountainTerrain.SetSeed(time(nullptr));
	mountainTerrain.SetFrequency(0.5);
	utils::NoiseMap heightMap;
	static const size_t width = 128;
	static const size_t height = 128;
	utils::NoiseMapBuilderPlane heightMapBuilder;
	heightMapBuilder.SetSourceModule(mountainTerrain);
	heightMapBuilder.SetDestNoiseMap(heightMap);
	heightMapBuilder.SetDestSize(width, height);
	heightMapBuilder.SetBounds(0.0, 4.0, 0.0, 4.0);
	heightMapBuilder.Build();
	MeshBuilder meshBuilder(width, height);
	int vertexIndex = 0;
	static const float spacing = 0.5f;
	static const float elevScale = 5.f;
        std::array<std::array<glm::vec3, 128>, 128> vertices;
	for (size_t y = 0; y < height; ++y) {
	    for (size_t x = 0; x < width; ++x) {
		glm::vec3 vert = {x * spacing, heightMap.GetValue(x, y) * elevScale, y * spacing};
		meshBuilder.AddVertex(vertexIndex, vert);
		vertices[y][x] = vert;
		if (x < width - 1 && y < height - 1) {
		    meshBuilder.AddTriangle(vertexIndex,
					    vertexIndex + width + 1, vertexIndex + width);
		    meshBuilder.AddTriangle(vertexIndex + width + 1, vertexIndex,
					    vertexIndex + 1);
		}
		++vertexIndex;
	    }
	}
	std::array<std::array<glm::vec3, 128>, 128> normals{};
	for (size_t y = 0; y < height; ++y) {
	    for (size_t x = 0; x < width; ++x) {
		if (x < width - 1 && y < height - 1) {
		    glm::vec3 p0 = vertices[y][x];
		    glm::vec3 p1 = vertices[y + 1][x];
		    glm::vec3 p2 = vertices[y][x + 1];
		    glm::vec3 e1 = p1 - p0;
		    glm::vec3 e2 = p2 - p0;
		    glm::vec3 normal = glm::cross(e1, e2);
		    normal = glm::normalize(normal);
		    normals[y][x] += normal;
		    normals[y + 1][x] += normal;
		    normals[y][x + 1] += normal;

		    p0 = vertices[y + 1][x + 1];
		    e1 = p1 - p0;
		    e2 = p2 - p0;
		    normal = glm::cross(e2, e1);
		    normal = glm::normalize(normal);
		    normals[y + 1][x + 1] += normal;
		    normals[y + 1][x] += normal;
		    normals[y][x + 1] += normal;
		}
	    }
	}
	std::vector<glm::vec3> linearNorms;
	for (size_t i = 0; i < height; ++i) {
	    std::copy(normals[i].begin(), normals[i].end(), std::back_inserter(linearNorms));
	}
	for (auto & element : linearNorms) {
	    element = glm::normalize(element);
	    // std::cout << element.x << " " << element.y << " " << element.z << "\n";
	}
	auto mesh = meshBuilder.GetMesh();
	glGenBuffers(1, &terrainIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.triangles.size() * sizeof(GLushort),
		     mesh.triangles.data(), GL_STATIC_DRAW);
	indexCount = mesh.triangles.size();
	glGenBuffers(1, &terrainData);
	glBindBuffer(GL_ARRAY_BUFFER, terrainData);
	std::vector<TerrainVert> data;
	for (size_t i = 0; i < mesh.vertices.size(); ++i) {
	    data.push_back({mesh.vertices[i], linearNorms[i]});
	}
	terrainDataSize = data.size() * sizeof(TerrainVert);
	glBufferData(GL_ARRAY_BUFFER, terrainDataSize, data.data(), GL_STATIC_DRAW);
    }

    struct TerrainVert {
	glm::vec3 pos;
	glm::vec3 norm;
    };
    
    void Run() {
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
	try {
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
	} catch (const std::exception & ex) {
	    m_running = false;
	    logicThread.join();
	    throw std::runtime_error(ex.what());
	}
	logicThread.join();
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
