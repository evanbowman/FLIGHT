#pragma once

#include <OpenGL/gl3.h>

struct BlendMode {
    enum class Mode {
	Zero          = GL_ZERO,
	One           = GL_ONE,
	Alpha         = GL_SRC_ALPHA,
	OneMinusAlpha = GL_ONE_MINUS_SRC_ALPHA,
    };
    Mode src, dest;
};
