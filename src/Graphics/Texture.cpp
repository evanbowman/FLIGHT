#include <FLIGHT/Graphics/Texture.hpp>
#include <iostream>

#define NANOSVG_IMPLEMENTATION
#include <nanosvg/nanosvg.h>
#define NANOSVGRAST_IMPLEMENTATION
#include <nanosvg/nanosvgrast.h>

namespace FLIGHT {
void Texture::Cleanup() {
    if (m_loaded) {
        glDeleteTextures(1, &m_id);
    }
}

void Texture::CreateFromSFImage(sf::Image & img, Sampling sampling) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getSize().x, img.getSize().y, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    static_cast<int>(sampling));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    static_cast<int>(sampling));
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::LoadFromSVG(const std::string & path, const float scale) {
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    static struct SVGRasterizer {
        NSVGrasterizer * engine;
        SVGRasterizer() {
            engine = nsvgCreateRasterizer();
            if (not engine) {
                throw std::runtime_error("Failed to alloc svg rasterizer");
            }
        }
        ~SVGRasterizer() { nsvgDeleteRasterizer(engine); }
    } svgRasterizer;
    auto svgImage = nsvgParseFromFile(path.c_str(), "px", 96.f);
    if (not svgImage) {
        throw std::runtime_error("Failed to load svg image from: " + path);
    }
    int w = static_cast<int>(svgImage->width * scale);
    int h = static_cast<int>(svgImage->width * scale);
    sf::Image sfImage;
    sfImage.create(w, h);
    nsvgRasterize(svgRasterizer.engine, svgImage, 0, 0, scale,
                  (unsigned char *)sfImage.getPixelsPtr(), w, h, w * 4);
    nsvgDelete(svgImage);
    CreateFromSFImage(sfImage, Sampling::Nearest);
    m_loaded = true;
}

void Texture::LoadFromRaster(const std::string & path, Sampling sampling) {
    Cleanup();
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    sf::Image img;
    if (not img.loadFromFile(path)) {
        throw std::runtime_error("Failed to load image from: " + path);
    }
    CreateFromSFImage(img, sampling);
    m_loaded = true;
}

void Texture::LoadFromMemory(const std::vector<uint8_t> & data,
                             const glm::vec2 & size) {
    Cleanup();
    glGenTextures(1, &m_id);
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
