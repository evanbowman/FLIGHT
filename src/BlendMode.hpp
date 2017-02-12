#pragma once

#include <OpenGL/gl3.h>

struct BlendMode {
    enum class Mode {
	Zero = GL_ZERO,
	One  = GL_ONE,
    };
    Mode src, dest;
};
