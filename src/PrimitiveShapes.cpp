#include "PrimitiveShapes.hpp"

GLuint Primitives::QuadBase::m_vbo;

GLuint Primitives::Hexagon::m_vbo;

void Primitives::Init() {
    // Note: for the sake of compactness, all primitive shapes
    // are triangle fans.
    static const float basicQuad[] = {
	1.f,  1.f, 0.0f,   1.0f, 1.0f,
	1.f, -1.f, 0.0f,   1.0f, 0.0f,
	-1.f, -1.f, 0.0f,   0.0f, 0.0f,
	-1.f,  1.f, 0.0f,   0.0f, 1.0f
    };
    glGenBuffers(1, &QuadBase::m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, QuadBase::m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(basicQuad), basicQuad, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Note: I don't have it set up so that you can texture a hexagon... but
    // why would you want to!?
    // FIXME: make hexes unit length...
    static const float basicHex[] = {
	-10.f, -17.321f, 0.f,
	-20.f, 0.f, 0.f,
	-10.f, 17.321f, 0.f,
	10.f, 17.321f, 0.f,
	20.f, 0.f, 0.f,
	10.f, -17.321f, 0.f
    };
    glGenBuffers(1, &Hexagon::m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, Hexagon::m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(basicHex), basicHex, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Primitives::Quad::Display(ShaderProgram & shader, const BlendMode & blendMode) {
    GLint posAttrib = glGetAttribLocation(shader.GetHandle(), "position");
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glBlendFunc(static_cast<GLenum>(blendMode.src),
		static_cast<GLenum>(blendMode.dest));
    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
    glBlendFunc(GL_ONE, GL_ZERO);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Primitives::TexturedQuad::Display(ShaderProgram & shader, const BlendMode & blendMode) {
    GLint posAttrib = glGetAttribLocation(shader.GetHandle(), "position");
    GLint texAttrib = glGetAttribLocation(shader.GetHandle(), "texCoord");
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glBlendFunc(static_cast<GLenum>(blendMode.src),
		static_cast<GLenum>(blendMode.dest));
    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
    glBlendFunc(GL_ONE, GL_ZERO);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Primitives::Hexagon::Display(ShaderProgram & shader, const BlendMode & blendMode) {
    GLint posAttrib = glGetAttribLocation(shader.GetHandle(), "position");
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBlendFunc(static_cast<GLenum>(blendMode.src),
		static_cast<GLenum>(blendMode.dest));
    glDrawArrays(GL_TRIANGLE_FAN, 0, 12);
    glBlendFunc(GL_ONE, GL_ZERO);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
