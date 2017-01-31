#include "Camera.hpp"

void Camera::SetTarget(std::shared_ptr<Entity> target) {
    m_target = target;
    m_currentRot = target->GetRotation().y;
}

void Camera::Update() {
    if (auto sharedTarget = m_target.lock()) {
	glm::vec3 cameraTarget = sharedTarget->GetPosition();
        static const glm::vec3 cameraUp(0, 1, 0);
	glm::vec3 cameraPosition = cameraTarget;
	auto targetRot = sharedTarget->GetRotation();
	// cameraPosition.z += std::cos(targetRot.y) * 2;
	// cameraPosition.x += std::sin(targetRot.y) * 2;
	m_currentRot = math::lerp(targetRot.y, m_currentRot, 0.001);
	cameraPosition.z += std::cos(m_currentRot) * 2;
	cameraPosition.x += std::sin(m_currentRot) * 2;
	// if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
	//     cameraTarget.z += 1;
	//     cameraPosition.z -= 2.6;
	// } else {
	    // cameraTarget.z -= 1;
	    // cameraPosition.z += 2.6;
	// }
	cameraPosition.y += 1.6;
	m_view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
    }
}

glm::mat4 Camera::GetView() const {
    return m_view;
}
