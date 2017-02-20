#include "Font.hpp"

static FT_Library g_ft;

void Font::Init() {
    int error = FT_Init_FreeType(&g_ft);
    if (error) {
	throw std::runtime_error("failed to initialize the FreeType library");
    }
}

Font::Font() : m_face(nullptr) {}

void Font::CreateFaceFromFile(const std::string & path) {
    if (!g_ft) {
	throw std::runtime_error("missing freetype context");
    }
    if (m_face) {
	throw std::runtime_error("attempt to create multiple faces in a single Font instance");
    }
    if (FT_New_Face(g_ft, path.c_str(), 0, &m_face)) {
	throw std::runtime_error("failed to create font face from " + path);
    }
}

void Font::CreateGlyphSet(const int glyphHeight) {
    if (!m_face) {
	throw std::runtime_error("attempt to create glyph set before loading face");
    }
    FT_Set_Pixel_Sizes(m_face, 0, glyphHeight);
    
}
