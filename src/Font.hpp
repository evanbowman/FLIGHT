#pragma once

#include <stdexcept>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_BITMAP_H

class Font {
    friend class Game;
    static void Init();
    FT_Face m_face;
public:
    Font();
    void CreateFaceFromFile(const std::string & path);
    void CreateGlyphSet(const int glyphHeight);
};
