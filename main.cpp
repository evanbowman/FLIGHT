#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
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
#include <cassert>

struct Vec3 {
    float x, y, z;
};

struct Vec4 {
    float w, x, y, z;
};

struct Vec5 {
    float x, y, z, v, w;
};

struct Face {
    int quad[4];
    int texCoords[4];
    int faceNumber;
};

struct Vec2 {
    float x, y;
};

struct Vertex {
    Vec3 pos;
    Vec3 norm;
    Vec2 tex;
};

class Model {
    std::vector<Vec3> m_vertices;
    std::vector<Vec3> m_normals;
    std::vector<Vec2> m_texCoords;
    std::vector<Face> m_faces;
public:
    void LoadFromWavefront(const std::string & path) {
	m_vertices.clear();
	m_normals.clear();
	m_faces.clear();
	std::fstream file(path);
	std::string line;
	while (std::getline(file, line)) {
	    std::string prefix;
	    std::copy(line.begin(), line.begin() + 2, std::back_inserter(prefix));
	    std::stringstream ss(line);
	    if (prefix == "v ") {
		float x, y, z;
	        if (ss >> prefix >> x >> y >> z) {
		    m_vertices.push_back({x, y, z});
		}
	    } else if (prefix == "vn") {
		float x, y, z;
		if (ss >> prefix >> x >> y >> z) {
		    m_normals.push_back({x, y, z});
		}
	    } else if (prefix == "vt") {
		float x, y;
		if (ss >> prefix >> x >> y) {
		    m_texCoords.push_back({x, y});
		}
		// I'm not sure what this data is for yet...
	    } else if (prefix == "f ") {
		int i0, i1, i2, i3;
		int t0, t1, t2, t3;
		int n;
		int resCount = sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
		       &i0, &t0, &n,
		       &i1, &t1, &n,
		       &i2, &t2, &n,
		       &i3, &t3, &n);
		if (resCount != 12) {
		    throw std::runtime_error("Invalid format, failed to load: " + path);
		}
		m_faces.push_back({{i0, i1, i2, i3},
				   {t0, t1, t2, t3}, n});
	    }
	}
    }

    const std::vector<Vec2> & GetTexCoords() const {
	return m_texCoords;
    }

    const std::vector<Vec3> & GetVertices() const {
	return m_vertices;
    }

    const std::vector<Vec3> & GetNormals() const {
	return m_normals;
    }

    const std::vector<Face> & GetFaces() const {
	return m_faces;
    }
};

class Texture {
    GLuint m_id;
    friend class DisplayList;
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
    }
};

class DisplayList {
    int m_id, m_texture;
    
public:
    void SetTexture(const Texture & texture) {
	m_texture = texture.m_id;
    }
    
    explicit DisplayList(const Model & model) : m_id(glGenLists(1)) {
	const auto & vertices = model.GetVertices();
	const auto & normals = model.GetNormals();
	const auto & faces = model.GetFaces();
	const auto & texCoords = model.GetTexCoords();
	glNewList(m_id, GL_COMPILE);
	for (size_t i = 0; i < faces.size(); ++i) {
	    glBegin(GL_QUADS);
	    glNormal3f(normals[faces[i].faceNumber - 1].x,
		       normals[faces[i].faceNumber - 1].y,
		       normals[faces[i].faceNumber - 1].z);
	    glTexCoord2f(texCoords[faces[i].texCoords[0] - 1].x,
			 texCoords[faces[i].texCoords[0] - 1].y);
	    glVertex3f(vertices[faces[i].quad[0] - 1].x,
		       vertices[faces[i].quad[0] - 1].y,
		       vertices[faces[i].quad[0] - 1].z);
	    glTexCoord2f(texCoords[faces[i].texCoords[1] - 1].x,
			 texCoords[faces[i].texCoords[1] - 1].y);
	    glVertex3f(vertices[faces[i].quad[1] - 1].x,
		       vertices[faces[i].quad[1] - 1].y,
		       vertices[faces[i].quad[1] - 1].z);
	    glTexCoord2f(texCoords[faces[i].texCoords[2] - 1].x,
			 texCoords[faces[i].texCoords[2] - 1].y);
	    glVertex3f(vertices[faces[i].quad[2] - 1].x,
		       vertices[faces[i].quad[2] - 1].y,
		       vertices[faces[i].quad[2] - 1].z);
	    glTexCoord2f(texCoords[faces[i].texCoords[3] - 1].x,
			 texCoords[faces[i].texCoords[3] - 1].y);
	    glVertex3f(vertices[faces[i].quad[3] - 1].x,
		       vertices[faces[i].quad[3] - 1].y,
		       vertices[faces[i].quad[3] - 1].z);
	    glEnd();
	}
	glEndList();
    }

    void Draw() {
	glCallList(m_id);
    }
};

class App {
    sf::Window m_window;
    int m_framerate;
public:
    App(const std::string & name) :
	m_window(sf::VideoMode::getDesktopMode(), name.c_str(), sf::Style::Fullscreen,
		 sf::ContextSettings(24, 8, 4, 0, 1)),
	m_framerate(120) {
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
        glClearColor(0.03f, 0.41f, 0.58f, 1.f);
	m_window.setMouseCursorVisible(false);
    }
    
    int Run() {
	Model wingModel;
        wingModel.LoadFromWavefront("wing.obj");
	Texture texture;
	texture.LoadFromFile("wingTexture.png");
	DisplayList wingDL(wingModel);
	bool running = true;
	sf::Clock clock;
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
	    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		glRotatef(0.5f, 1.f, 0.f, 1.f);
	    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		glRotatef(-0.5f, 1.f, 0.f, 1.f);
	    }
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    wingDL.Draw();
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
