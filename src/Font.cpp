#include "Font.hpp"

static FT_Library g_FTLibrary;

void Font::Init() {
    int error = FT_Init_FreeType(&g_FTLibrary);
    if (error) {
	throw std::runtime_error("Error: failed to initialize the FreeType library");
    }
}
