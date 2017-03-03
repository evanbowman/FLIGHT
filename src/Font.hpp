#pragma once

//#include <ft2build.h>
//#include FT_FREETYPE_H
#include <stdexcept>
#include <memory>

class FontFace {
    friend class Game;
    static void Init();
public:
    static std::shared_ptr<FontFace> New(const std::string & path, size_t size);
};
