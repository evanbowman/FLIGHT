#pragma once

#ifdef FLIGHT_MAC
#include <OpenGL/gl3.h>
#elif FLIGHT_WINDOWS
#include <GL/glew.h>
#endif
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

namespace FLIGHT {
class Texture {
    GLuint m_id;

public:
    enum class Sampling { Nearest = GL_NEAREST, Linear = GL_LINEAR };
    void LoadFromFile(const std::string & path,
                      Sampling sampling = Sampling::Nearest);
    GLuint GetId() const;
    const Texture & operator=(const Texture &) = delete;
    ~Texture();
};
}
