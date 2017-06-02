#pragma once

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include "BlendFunc.hpp"
#include <FLIGHT/Graphics/Error.hpp>
#include "Shader.hpp"

namespace FLIGHT {
namespace PRIMITIVES {
class QuadBase {
protected:
    static GLuint m_vbo;
    friend void Init();
};

class Quad : public QuadBase {
public:
    void Display(ShaderProgram & shader, const BlendFunc & blendFunc);
};

class TexturedQuad : public QuadBase {
public:
    void Display(ShaderProgram & shader, const BlendFunc & blendFunc);
};

class Hexagon {
    static GLuint m_vbo;
    friend void Init();

public:
    void Display(ShaderProgram & shader, const BlendFunc & blendFunc);
};

void Init();
}
}
