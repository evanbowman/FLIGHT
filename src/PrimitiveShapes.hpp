#pragma once

#include <OpenGL/gl3.h>
#include "BlendMode.hpp"
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
	void Display(ShaderProgram & shader, const BlendMode & blendMode);
    };

    class TexturedQuad : public QuadBase {
    public:
	void Display(ShaderProgram & shader, const BlendMode & blendMode);
    };
    
    class Hexagon {
	static GLuint m_vbo;
	friend void Init();
    public:
	void Display(ShaderProgram & shader, const BlendMode & blendMode);
    };
    
    void Init();
}
