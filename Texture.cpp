#include "Texture.hpp"

void Texture::LoadFromFile(const std::string & path) {
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

GLuint Texture::GetId() const {
    return m_id;
}
