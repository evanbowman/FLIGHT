#include "Reticle.hpp"
#include "Game.hpp"
#include <iostream>

void Reticle::Update(const Player & player) {
    if (auto plane = player.GetPlane()) {
	m_position = plane->GetPosition();
	const glm::vec3 planeRot = plane->GetRotation();
	glm::mat4 yawPitchRotMat;
        yawPitchRotMat = glm::rotate(yawPitchRotMat, planeRot.y, 
				     glm::vec3(0.0f, 1.0f, 0.0f));
        yawPitchRotMat = glm::rotate(yawPitchRotMat, planeRot.x, 
				     glm::vec3(1.0f, 0.0f, 0.0f));
	glm::vec3 dir =
	    glm::normalize(-1.0f * glm::vec3(yawPitchRotMat[2].x,
					     yawPitchRotMat[2].y,
					     yawPitchRotMat[2].z));
	static const float RETICLE_DIST_FROM_PLANE_CENTROID(2.5f);
	m_position += RETICLE_DIST_FROM_PLANE_CENTROID * dir;
	auto windowSize = GetGame().GetWindowSize();
	glm::mat4 view = GetGame().GetCamera().GetWorldView();
	glm::mat4 proj = glm::perspective(45.0f, windowSize.x / (float)windowSize.y, 0.1f, 1.0f);
	glm::vec4 viewPort = {0, 0, windowSize.x, windowSize.y};
	m_position = glm::project(m_position, view, proj, viewPort);
	m_position.z = 0.f;
    }
}

void Reticle::Display() {
    const GLuint reticleProg = GetGame().GetAssets().GetShaderProgram(ShaderProgramId::Reticle);
    glUseProgram(reticleProg);
    Primitives::Quad quad;
    glm::mat4 model = glm::translate(glm::mat4(1), m_position);
    model = glm::scale(model, {30.f, 30.f, 0.f});
    const GLint modelLoc = glGetUniformLocation(reticleProg, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    quad.Display(reticleProg, {BlendMode::Mode::One, BlendMode::Mode::One});
}
