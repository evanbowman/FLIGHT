#include "Camera.hpp"

namespace FLIGHT {
void Camera::SetTarget(std::shared_ptr<Entity> target) {
    m_target = target;
    m_currentRotY = target->GetRotation().y;
    m_currentRotX = target->GetRotation().x;
    m_shiftAmount = 0.f;
}

static const glm::vec3 cameraUp(0, 1, 0);

void Camera::Update(const Time dt) {
    if (auto sharedTarget = m_target.lock()) {
        glm::vec3 cameraTarget = sharedTarget->GetPosition();
        m_lightView = glm::lookAt(
            {cameraTarget.x, cameraTarget.y + 4, cameraTarget.z - 1},
            cameraTarget, cameraUp);
        glm::vec3 cameraPosition = cameraTarget;
        auto targetRot = sharedTarget->GetRotation();
        m_currentRotY = math::lerp(targetRot.y, m_currentRotY, 0.000002f * dt);
        m_currentRotX = math::lerp(targetRot.x, m_currentRotX, 0.000001f * dt);
        m_shiftAmount =
            math::lerp(targetRot.z, m_shiftAmount, 0.00000005f * dt);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
            cameraPosition.z -= std::cos(m_currentRotY) * 2.4f;
            cameraPosition.x -= std::sin(m_currentRotY) * 2.4f;
        } else {
            cameraPosition.z += std::cos(m_currentRotY) * 2.4f;
            cameraPosition.x += std::sin(m_currentRotY) * 2.4f;
        }
        cameraPosition.x -= std::cos(targetRot.y) * m_shiftAmount * 1.5f;
        cameraTarget.x -= std::cos(targetRot.y) * m_shiftAmount * 1.5f;
        cameraPosition.z += std::sin(targetRot.y) * m_shiftAmount * 1.5f;
        cameraTarget.z += std::sin(targetRot.y) * m_shiftAmount * 1.5f;

        cameraPosition.y -= std::sin(m_currentRotX) * 2.f;
        m_shiftAmount = math::lerp(targetRot.z, m_shiftAmount, 0.000001f * dt);
        cameraPosition.y += 1.4;
        cameraTarget.y += 0.3;
        m_cameraView = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
        m_viewDir = cameraPosition - cameraTarget;
    }
}

const glm::vec3 & Camera::GetViewDir() const { return m_viewDir; }

const glm::mat4 & Camera::GetWorldView() const { return m_cameraView; }

const glm::mat4 & Camera::GetLightView() const { return m_lightView; }
}
