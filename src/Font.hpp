#pragma once

#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED

#include "../deps/freetype-gl/freetype-gl.h"
#include "Error.hpp"
#include "BlendMode.hpp"

#include <initializer_list>
#include <glm/glm.hpp>
#include <vector>
#include <string>

enum class FontSizeFactor {
    Large, Medium, Small
};

int CalcFontSize(FontSizeFactor size);

struct Font {
    texture_font_t * texture;
    int size;
};

class FontAtlas {
    texture_atlas_t * m_texAtlas;
    std::vector<Font> m_fonts;
public:
    void LoadFromFile(const std::string & path,
		      const std::initializer_list<int> & sizes);
    const Font & GetFont(FontSizeFactor size) const;
    GLuint GetTexture() const;
};

class Text {
    std::weak_ptr<FontAtlas> m_fontAtlas;
    GLuint m_vbo;
    size_t m_bufSize;
    bool m_initialized;
public:
    Text();
    Text(const Text & other) = delete;
    const Text & operator=(const Text & other) = delete;
    Text(Text && other);
    const Text & operator=(Text && other);
    void SetFontAtlas(std::shared_ptr<FontAtlas> atlas);
    void Create(const std::string & msg, FontSizeFactor size);
    void Display(const GLuint shaderProg, const BlendMode & blendMode);
    ~Text();
};
