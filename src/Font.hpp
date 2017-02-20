#pragma once

#include <stdexcept>
#include <ft2build.h>
#include FT_FREETYPE_H

class Font {
    // ...
    friend class Game;
    static void Init();
public:
    Font(const Font &) = delete;
    const Font & operator=(const Font &) = delete;
};
