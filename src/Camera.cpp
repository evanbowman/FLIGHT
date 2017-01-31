#include "Camera.hpp"

void Camera::SetTarget(std::shared_ptr<Entity> target) {
    m_target = target;
}

void Camera::Update() {
    if (auto sharedTarget = m_target.lock()) {
	glm::vec3 cameraTarget = sharedTarget->GetPosition();
        static const glm::vec3 cameraUp(0, 1, 0);
	glm::vec3 cameraPosition = cameraTarget;
	cameraTarget.z -= 1;
	cameraPosition.z += 2.6;
	cameraPosition.y += 1.6;
	m_view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
    }
}

glm::mat4 Camera::GetView() const {
    return m_view;
}
