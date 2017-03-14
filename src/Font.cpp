#include "Font.hpp"
#include "Game.hpp"

#include "ResourcePath.hpp"
#include <SFML/Graphics.hpp>

namespace FLIGHT {
std::shared_ptr<FontFace> FontFace::New(const std::string & path, size_t size) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        throw std::runtime_error("failed to initialize freetype");
    }
    auto fontFace = std::shared_ptr<FontFace>(new FontFace);
    FT_Face face;
    if (FT_New_Face(ft, path.c_str(), 0, &face)) {
        throw std::runtime_error("Failed to open font from file: " + path);
    }
    FT_Set_Pixel_Sizes(face, 0, size);
    for (int i = 32; i < 126; ++i) {
        if (FT_Load_Char(face, static_cast<char>(i), FT_LOAD_RENDER)) {
            throw std::runtime_error("Freetype failed to load glyph \'" +
                                     std::string(1, (char)i) +
                                     "\' from file: " + path);
        }
        auto g = face->glyph;
        glGenTextures(1, &fontFace->m_glyphs[i].texture);
        glBindTexture(GL_TEXTURE_2D, fontFace->m_glyphs[i].texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, g->bitmap.width, g->bitmap.rows,
                     0, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
	fontFace->m_glyphs[i].size = {
	    g->bitmap.width, g->bitmap.rows
	};
	fontFace->m_glyphs[i].bearing = {
	    g->bitmap_left, g->bitmap_top
	};
	fontFace->m_glyphs[i].advance = g->advance.x;
    }
    AssertGLStatus("glyph generation");
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    return fontFace;
}

void Text::Display() {
    if (auto fontFaceSp = m_face.lock()) {
	auto & glyphs = fontFaceSp->GetGlyphs();
	glActiveTexture(GL_TEXTURE1);
	auto & fontShader = GetGame().GetAssetMgr().GetProgram<ShaderProgramId::Font>();
	fontShader.Use();
	fontShader.SetUniformInt("tex", 1);
	size_t mark = 0;
	for (const char c : m_string) {
	    auto & glyph = glyphs[static_cast<int>(c)];
	    glBindTexture(GL_TEXTURE_2D, glyph.texture);
	    PRIMITIVES::TexturedQuad quad;
	    glm::mat4 model;
	    model = glm::translate(model, {m_position.x + mark + glyph.bearing.x, m_position.y - (glyph.size.y - glyph.bearing.y), 0.f});
	    model = glm::scale(model, {static_cast<float>(glyph.size.x * 2), static_cast<float>(glyph.size.y *), 1.f});
	    fontShader.SetUniformMat4("model", model);
	    quad.Display(fontShader, AlphaBlend);
	    mark += (glyph.advance >> 6);
	    glBindTexture(GL_TEXTURE_2D, 0);
	}
    } else {
	throw std::runtime_error("Attempt to display text without FontFace");
    }
}

const std::string & Text::GetString() const {
    return m_string;
}

void Text::SetString(const std::string & string) {
    m_string = string;
}

void Text::SetPosition(const glm::vec3 & position) {
    m_position = position;
}

void Text::SetFace(std::shared_ptr<FontFace> face) {
    m_face = face;
}
}
