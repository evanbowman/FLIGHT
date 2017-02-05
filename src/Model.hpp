#pragma once

#include <vector>
#include <OpenGL/gl3.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <array>
#include "Vertex.hpp"

class Model {
    GLuint m_vbo = 0;
    size_t m_vertCoordSize;

public:
    friend class App;
    void LoadFromWavefront(const std::string & path);
    size_t BindAll(const GLuint shaderProgram);
    size_t BindVertices(const GLuint shaderProgram);
};
