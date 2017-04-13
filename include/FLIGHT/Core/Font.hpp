#pragma once

#include <array>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <FLIGHT/Graphics/Color.hpp>
#include "Error.hpp"
#include <FLIGHT/Graphics/PrimitiveShapes.hpp>

namespace FLIGHT {
class FontFace {
    FontFace() {}
    struct Glyph {
        GLuint texture;
        glm::ivec2 size;
        glm::ivec2 bearing;
        size_t advance;
    };
    std::array<Glyph, 128> m_glyphs;

public:
    static std::shared_ptr<FontFace> New(const std::string & path, size_t size);
    const decltype(m_glyphs) & GetGlyphs() const { return m_glyphs; }
};

class Text {
    std::string m_string;
    std::weak_ptr<FontFace> m_face;
    glm::vec3 m_position;
    glm::ivec2 m_size;
    Color m_color;
    friend class Game;
    static GLuint m_vbo;
    void RecalcSize();

public:
    static void Enable();
    Text();
    const std::string & GetString() const;
    void Display();
    void SetColor(const Color & color);
    Color GetColor() const;
    void SetString(const std::string & string);
    void SetFace(std::shared_ptr<FontFace> face);
    void SetPosition(const glm::vec3 & position);
    const glm::vec3 & GetPosition() const;
    const glm::ivec2 & GetSize() const;
};
}
