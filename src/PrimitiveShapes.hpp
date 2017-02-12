#pragma once

#include <OpenGL/gl3.h>
#include "BlendMode.hpp"
#include "Error.hpp"

namespace Primitives {
    class Quad {
	static GLuint m_vbo, m_ebo;
	friend void Init();
    public:
	void Display(const GLuint shaderProgram, const BlendMode & blendMode);
    };

    class Hexagon {
	static GLuint m_vbo, m_ebo;
	friend void Init();
    public:
	void Display(const GLuint shaderProgram, const BlendMode & blendMode);
    };
    
    void Init();
}
