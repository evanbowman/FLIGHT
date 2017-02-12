#include "Sky.hpp"
#include "Game.hpp"

static const float g_basicQuad[] = {
     20.f,  20.f, 0.0f,   1.0f, 1.0f,
     20.f, -20.f, 0.0f,   1.0f, 0.0f,
    -20.f, -20.f, 0.0f,   0.0f, 0.0f,
    -20.f,  20.f, 0.0f,   0.0f, 1.0f
};

GLushort g_basicQuadIndices[] = {
    0, 1, 3,
    1, 2, 3
};

SkyManager::SkyManager() {
    glGenBuffers(1, &m_quadVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_basicQuad), g_basicQuad, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenBuffers(1, &m_quadEbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quadEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_basicQuadIndices), g_basicQuadIndices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void SkyManager::Update(const glm::vec3 & cameraPos, const glm::vec3 & viewDir) {
    m_sunPos = cameraPos - glm::vec3{0, 0, 400};
    m_sunPos.y = 180;
    m_sunVisible = IntersectsFrustum(m_sunPos, cameraPos, viewDir);
}

void SkyManager::Display(const GLuint) {
    if (m_sunVisible) {
	const GLuint textrdQuadProg =
	    GetGame().GetAssets().GetShaderProgram(ShaderProgramId::GenericTextured);
	glUseProgram(textrdQuadProg);
	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(textrdQuadProg, "tex"), 1);
	glBindTexture(GL_TEXTURE_2D, GetGame().GetAssets().GetTexture(TextureId::Sun)->GetId());
	const GLint modelLoc = glGetUniformLocation(textrdQuadProg, "model");
	GLint posAttrib = glGetAttribLocation(textrdQuadProg, "position");
	GLint texAttrib = glGetAttribLocation(textrdQuadProg, "texCoord");
	glm::mat4 model;
	model = glm::translate(model, m_sunPos);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindBuffer(GL_ARRAY_BUFFER, m_quadVbo);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quadEbo);
	glBlendFunc(GL_ONE, GL_ONE);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	glBlendFunc(GL_ONE, GL_ZERO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
    }
}
