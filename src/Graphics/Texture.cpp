#include <FLIGHT/Graphics/Texture.hpp>

namespace FLIGHT {
void Texture::Cleanup() {
    if (m_loaded) {
        glDeleteTextures(1, &m_id);
    }
}

void Texture::LoadFromFile(const std::string & path, Sampling sampling) {
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    sf::Image img;
    if (!img.loadFromFile(path)) {
        std::cerr << "Image loading failed...\n";
        return;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getSize().x, img.getSize().y, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    static_cast<int>(sampling));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    static_cast<int>(sampling));
    glBindTexture(GL_TEXTURE_2D, 0);
    m_loaded = true;
}

void Texture::LoadFromMemory(const std::vector<uint8_t> & data,
                             const glm::vec2 & size) {
    Cleanup();
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    m_loaded = true;
}

GLuint Texture::GetId() const { return m_id; }

Texture::~Texture() { Cleanup(); }
}
