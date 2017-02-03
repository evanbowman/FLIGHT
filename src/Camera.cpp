#include "Camera.hpp"

void Camera::SetTarget(std::shared_ptr<Entity> target) {
    m_target = target;
    m_currentRotY = target->GetRotation().y;
    m_currentRotX = target->GetRotation().x;
    m_shiftAmount = 0.f;
}

void Camera::Update(const long long dt) {
    if (auto sharedTarget = m_target.lock()) {
	glm::vec3 cameraTarget = sharedTarget->GetPosition();
	glm::vec3 cameraPosition = cameraTarget;
	auto targetRot = sharedTarget->GetRotation();
	m_currentRotY = math::lerp(targetRot.y, m_currentRotY, 0.000002 * dt);
	m_currentRotX = math::lerp(targetRot.x, m_currentRotX, 0.000001 * dt);
	m_shiftAmount = math::lerp(targetRot.z, m_shiftAmount, 0.00000005 * dt);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
	    cameraPosition.z -= std::cos(m_currentRotY) * 2.1f;
	    cameraPosition.x -= std::sin(m_currentRotY) * 2.1f;
	} else {
	    cameraPosition.z += std::cos(m_currentRotY) * 2.1f;
	    cameraPosition.x += std::sin(m_currentRotY) * 2.1f;
	}
	cameraPosition.x -= std::cos(targetRot.y) * m_shiftAmount * 1.f;
	cameraTarget.x -= std::cos(targetRot.y) * m_shiftAmount * 1.f;
	cameraPosition.z += std::sin(targetRot.y) * m_shiftAmount * 1.f;
	cameraTarget.z += std::sin(targetRot.y) * m_shiftAmount * 1.f;
	
	cameraPosition.y -= std::sin(m_currentRotX) * 2.f;
	m_shiftAmount = math::lerp(targetRot.z, m_shiftAmount, 0.000001 * dt);
	cameraPosition.y += 1.8;
	static const glm::vec3 cameraUp(0, 1, 0);
	m_cameraView = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
    }
}

glm::mat4 Camera::GetCameraView() const {
    return m_cameraView;
}

glm::mat4 Camera::GetLightView() const {
    
}
