#pragma once

#include <vector>
#include <OpenGL/gl3.h>
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <array>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 norm;
    glm::vec2 texCoord;
};

class Model {
    GLuint m_vbo = 0;
    size_t m_vertCoordSize;

public:
    friend class App;
    void LoadFromWavefront(const std::string & path);
    size_t Bind(const GLuint shaderProgram);
};
