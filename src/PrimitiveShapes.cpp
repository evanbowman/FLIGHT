#include "PrimitiveShapes.hpp"

GLuint Primitives::Quad::m_vbo;
GLuint Primitives::Quad::m_ebo;

GLuint Primitives::Hexagon::m_vbo;
GLuint Primitives::Hexagon::m_ebo;

void Primitives::Init() {
    static const float g_basicQuad[] = {
	20.f,  20.f, 0.0f,   1.0f, 1.0f,
	20.f, -20.f, 0.0f,   1.0f, 0.0f,
	-20.f, -20.f, 0.0f,   0.0f, 0.0f,
	-20.f,  20.f, 0.0f,   0.0f, 1.0f
    };
    static const GLushort g_basicQuadIndices[] = {
	0, 1, 3,
	1, 2, 3
    };
    glGenBuffers(1, &Quad::m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, Quad::m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_basicQuad), g_basicQuad, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenBuffers(1, &Quad::m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Quad::m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_basicQuadIndices), g_basicQuadIndices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Primitives::Quad::Display(const GLuint shaderProgram, const BlendMode & blendMode) {
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    GLint texAttrib = glGetAttribLocation(shaderProgram, "texCoord");
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBlendFunc(static_cast<GLenum>(blendMode.src),
		static_cast<GLenum>(blendMode.dest));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    glBlendFunc(GL_ONE, GL_ZERO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
