#pragma once

#include <OpenGL/gl3.h>
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>

class Texture {
    GLuint m_id;
public:
    enum class Sampling {
	Nearest = GL_NEAREST,
	Linear = GL_LINEAR
    };
    void LoadFromFile(const std::string & path, Sampling sampling = Sampling::Nearest);
    GLuint GetId() const;
    const Texture & operator=(const Texture &) = delete;
    ~Texture();
};
