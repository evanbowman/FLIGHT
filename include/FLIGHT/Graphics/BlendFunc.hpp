#pragma once

#ifdef FLIGHT_MAC
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

namespace FLIGHT {
enum class BlendFactor {
    Zero = GL_ZERO,
    One = GL_ONE,
    SrcAlpha = GL_SRC_ALPHA,
    DstAlpha = GL_DST_ALPHA,
    DstColor = GL_DST_COLOR,
    SrcColor = GL_SRC_COLOR,
    ConstColor = GL_CONSTANT_COLOR,
    ConstAlpha = GL_CONSTANT_ALPHA,
    OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
    OneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA,
    OneMinusDstColor = GL_ONE_MINUS_DST_COLOR,
    OneMinusSrcColor = GL_ONE_MINUS_SRC_COLOR,
    OneMinusConstColor = GL_ONE_MINUS_CONSTANT_COLOR,
    OneMinusConstAlpha = GL_ONE_MINUS_CONSTANT_ALPHA,
};

struct BlendFunc {
    BlendFactor src, dest;
};

extern const BlendFunc AdditiveBlend;

extern const BlendFunc MultiplyBlend;

extern const BlendFunc AlphaBlend;
}
