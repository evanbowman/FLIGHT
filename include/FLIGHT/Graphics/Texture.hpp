#pragma once

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <string>

namespace FLIGHT {
class Texture {
public:
    enum class Sampling { Nearest = GL_NEAREST, Linear = GL_LINEAR };
    
private:
    GLuint m_id;
    bool m_loaded = false;
    void Cleanup();
    void CreateFromSFImage(sf::Image & img, Sampling sampling);

public:
    void LoadFromRaster(const std::string & path,
                        Sampling sampling = Sampling::Nearest);
    void LoadFromSVG(const std::string & path, const float scale);
    void LoadFromMemory(const std::vector<uint8_t> & data,
                        const glm::vec2 & size);
    GLuint GetId() const;
    const Texture & operator=(const Texture &) = delete;
    ~Texture();
};
}
