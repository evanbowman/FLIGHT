#include "Reticle.hpp"
#include "Game.hpp"
#include <iostream>

void Reticle::Update(const Player & player) {
    if (auto plane = player.GetPlane()) {
	m_position = plane->GetPosition();
	glm::vec3 dir = plane->GetForwardVec();
	static const float RETICLE_DIST_FROM_PLANE_CENTROID(2.5f);
	m_position += RETICLE_DIST_FROM_PLANE_CENTROID * dir;
	auto windowSize = GetGame().GetWindowSize();
	glm::mat4 view = GetGame().GetCamera().GetWorldView();
	glm::mat4 proj = glm::perspective(45.0f, windowSize.x /
					  (float)windowSize.y, 0.1f, 1.0f);
	glm::vec4 viewPort = {0, 0, windowSize.x, windowSize.y};
	m_position = glm::project(m_position, view, proj, viewPort);
	m_position.z = 0.f;
    }
}

void Reticle::DisplayImpl(const GLuint shaderProg) {
    glUseProgram(shaderProg);
    Primitives::TexturedQuad quad;
    glm::mat4 model = glm::translate(glm::mat4(1), m_position);
    model = glm::scale(model, {20.f, 20.f, 0.f});
    const GLint modelLoc = glGetUniformLocation(shaderProg, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    quad.Display(shaderProg, {BlendMode::Mode::Alpha, BlendMode::Mode::OneMinusAlpha});
}

void Reticle::Display() {
    const GLuint reticleProg =
	GetGame().GetAssetMgr().GetShaderProgram(ShaderProgramId::Reticle);
    const GLuint reticleShadowProg =
	GetGame().GetAssetMgr().GetShaderProgram(ShaderProgramId::ReticleShadow);
    DisplayImpl(reticleShadowProg);
    DisplayImpl(reticleProg);
}
