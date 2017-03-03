#include "Texture.hpp"

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
}

GLuint Texture::GetId() const { return m_id; }

Texture::~Texture() { glDeleteTextures(1, &m_id); }
