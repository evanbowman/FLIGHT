#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <unordered_map>
#include <string>
#include <OpenGL/gl.h>
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

struct Vertex {
    glm::vec3 pos;
    glm::vec3 norm;
    glm::vec2 texCoord;
};

class Model {
    GLuint m_vbo = 0;
    size_t m_vertCoordSize;

public:
    void LoadFromWavefront(const std::string & path) {
	std::vector<Vertex> vertexData;
	std::vector<glm::vec3> tempVertices;
	std::vector<glm::vec3> tempNormals;
	std::vector<glm::vec2> tempTexCoords;
	std::fstream file(path);
	size_t linum = 0;
	std::string line;
	while (std::getline(file, line)) {
	    std::string prefix;
	    std::copy(line.begin(), line.begin() + 2, std::back_inserter(prefix));
	    std::stringstream ss(line);
	    if (prefix == "v ") {
		float x, y, z;
	        if (ss >> prefix >> x >> y >> z) {
		    tempVertices.push_back({x, y, z});
		}
	    } else if (prefix == "vn") {
		float x, y, z;
		if (ss >> prefix >> x >> y >> z) {
		    tempNormals.push_back({x, y, z});
		}
	    } else if (prefix == "vt") {
		float x, y;
		if (ss >> prefix >> x >> y) {
		    tempTexCoords.push_back({x, y});
		}
	    } else if (prefix == "f ") {
		std::array<int, 3> v;
		std::array<int, 3> t;
		int n;
		int resCount = sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d",
		       &v[0], &t[0], &n,
		       &v[1], &t[1], &n,
		       &v[2], &t[2], &n);
		if (resCount != 9) {
		    std::cerr << linum << std::endl;
		    throw std::runtime_error("Invalid format, failed to load: " + path);
		}
		for (int i = 0; i < 3; ++i) {
		    vertexData.push_back({
			tempVertices[v[i] - 1],
			tempNormals[n - 1],
			tempTexCoords[t[i] - 1]
		    });
		}
	    }
	    ++linum;
	}
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	m_vertCoordSize = vertexData.size() * sizeof(Vertex);
	glBufferData(GL_ARRAY_BUFFER,
		     m_vertCoordSize,
		     vertexData.data(), GL_STATIC_DRAW);
    }

    void Display() {
	if (m_vbo == 0) {
	    return;
	}
	glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), 0);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void *)(2 * sizeof(glm::vec3)));
	glNormalPointer(GL_FLOAT, sizeof(Vertex), (void *)sizeof(glm::vec3));
	glDrawArrays(GL_TRIANGLES, 0, m_vertCoordSize);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
    }
};

class Texture {
    GLuint m_id;
public:
    void LoadFromFile(const std::string & path) {
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);
	sf::Image img;
	if (!img.loadFromFile(path)) {
	    std::cerr << "Image loading failed...\n";
	    return;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		     img.getSize().x, img.getSize().y,
		     0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
    }

    GLuint GetId() const {
	return m_id;
    }
};

class Sprite {
    std::weak_ptr<Texture> m_texture;
    std::weak_ptr<Model> m_model;
    glm::vec3 m_position;
    glm::vec3 m_scale;
public:
    Sprite() : m_position{}, m_scale{1, 1, 1} {}
    
    void SetTexture(std::shared_ptr<Texture> texture) {
	m_texture = texture;
    }

    void SetModel(std::shared_ptr<Model> model) {
	m_model = model;
    }

    void Display() {
	auto texSp = m_texture.lock();
	if (!texSp) {
	    throw std::runtime_error("Sprite missing texture data");
	}
	glBindTexture(GL_TEXTURE_2D, texSp->GetId());
	auto modSp = m_model.lock();
	if (!modSp) {
	    throw std::runtime_error("Sprite missing model data");
	}
	glTranslatef(m_position.x, m_position.y, m_position.z);
	glScalef(m_scale.x, m_scale.y, m_scale.z);
	modSp->Display();
	glScalef(1.f/m_scale.x, 1.f/m_scale.y, 1.f/m_scale.z);
	glTranslatef(-m_position.x, -m_position.y, -m_position.z);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void SetPosition(const glm::vec3 & position) {
	m_position = position;
    }

    void SetScale(const glm::vec3 & scale) {
	m_scale = scale;
    }
};

enum class TextureId {
    Wing, Engine, Count
};
enum class ModelId {
    Wing, Engine, Count
};

class AssetManager {
private:
    std::array<std::shared_ptr<Texture>, static_cast<int>(TextureId::Count)> m_textures;
    std::array<std::shared_ptr<Model>, static_cast<int>(ModelId::Count)> m_models;
public:
    void LoadTexture(const std::string & name, TextureId id) {
	auto textureSp = std::make_shared<Texture>();
	textureSp->LoadFromFile(name);
	m_textures[static_cast<int>(id)] = textureSp;
    }

    void LoadModel(const std::string & name, ModelId id) {
	auto modelSp = std::make_shared<Model>();
	modelSp->LoadFromWavefront(name);
	m_models[static_cast<int>(id)] = modelSp;
    }
    
    std::shared_ptr<Texture> GetTexture(TextureId id) {
	assert(m_textures[static_cast<int>(id)] != nullptr);
	return m_textures[static_cast<int>(id)];
    }

    std::shared_ptr<Model> GetModel(ModelId id) {
	assert(m_models[static_cast<int>(id)] != nullptr);
	return m_models[static_cast<int>(id)];
    }
};

class Plane {
    Sprite m_wing1, m_wing2, m_engine;
    glm::vec3 m_position;
public:
    Plane(AssetManager & assets) {
	m_wing1.SetTexture(assets.GetTexture(TextureId::Wing));
	m_wing2.SetTexture(assets.GetTexture(TextureId::Wing));
	m_wing1.SetModel(assets.GetModel(ModelId::Wing));
	m_wing2.SetModel(assets.GetModel(ModelId::Wing));
	m_engine.SetTexture(assets.GetTexture(TextureId::Engine));
	m_engine.SetModel(assets.GetModel(ModelId::Engine));
	m_engine.SetScale({0.07f, 0.07f, 0.07f});
	m_engine.SetPosition({0.f, 0.04f, 0.1f});
	m_wing1.SetScale({0.45f, 0.45f, 0.45f});
	m_wing2.SetScale({-0.45f, 0.45f, 0.45f});
	m_wing1.SetPosition({0.378f, 0.f, 0.f});
	m_wing2.SetPosition({-0.378f, 0.f, 0.f});
    }
    
    void Display() {
	m_engine.Display();
	m_wing1.Display();
	m_wing2.Display();
    }
};

class App {
    sf::Window m_window;
    int m_framerate;
    AssetManager m_assetManager;

    void LoadResources() {
	m_assetManager.LoadTexture("wingTexture.png", TextureId::Wing);
	m_assetManager.LoadTexture("engineTexture.png", TextureId::Engine);
	m_assetManager.LoadModel("wing.obj", ModelId::Wing);
	m_assetManager.LoadModel("engine.obj", ModelId::Engine);
    }
public:
    App(const std::string & name) :
	m_window(sf::VideoMode::getDesktopMode(), name.c_str(), sf::Style::Fullscreen,
		 sf::ContextSettings(24, 8, 4, 0, 1 // sf::ContextSettings::Default, true
				     )),
	m_framerate(120) {
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
        glClearColor(0.03f, 0.41f, 0.58f, 1.f);
	m_window.setMouseCursorVisible(false);
	this->LoadResources();
    }
    
    int Run() {
	Plane plane(m_assetManager);
	bool running = true;
	sf::Clock clock;
	const float aspect = static_cast<float>(m_window.getSize().x) /
	    static_cast<float>(m_window.getSize().y);
	glm::mat4 proj = glm::perspective(glm::radians(45.0f),
					  aspect, 1.0f, 10.0f);
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
	    glm::mat4 view = glm::lookAt(glm::vec3(1.2f, 1.2f, 1.2f),
					 glm::vec3(0.0f, 0.0f, 0.0f),
					 glm::vec3(0.0f, 0.0f, 1.0f));
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    plane.Display();
	    m_window.display();
	    assert(glGetError() == GL_NO_ERROR);
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
