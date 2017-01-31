#pragma once

#include <OpenGL/gl3.h>
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>

class Texture {
    GLuint m_id;
public:
    void LoadFromFile(const std::string & path);
    GLuint GetId() const;
};
