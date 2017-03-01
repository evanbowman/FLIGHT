#pragma once

#include <OpenGL/gl3.h>
#include "BlendFunc.hpp"
#include "Error.hpp"
#include "Shader.hpp"

namespace Primitives {
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
