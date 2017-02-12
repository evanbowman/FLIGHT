#include "Sky.hpp"
#include "Game.hpp"

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
	glm::mat4 model;
	// FIXME: Rotate the sun so that its normal points toward the eye
	model = glm::translate(model, m_sunPos);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	Primitives::Quad quad;
	quad.Display(textrdQuadProg, {BlendMode::Mode::One, BlendMode::Mode::One});
	glBindTexture(GL_TEXTURE_2D, 0);
    }
}
