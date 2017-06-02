#include <FLIGHT/Core/Camera.hpp>
#include <FLIGHT/Core/Game.hpp>

namespace FLIGHT {
void PlaneCamera::SetTarget(std::shared_ptr<Entity> target) {
    if (auto plane = std::dynamic_pointer_cast<Plane>(target)) {
        m_target = plane;
        m_currentRotY = target->GetRotation().y;
        m_currentRotX = target->GetRotation().x;
        m_shiftAmount = 0.f;
        m_yOff = plane->GetOBB().GetHeight() / 2.f;
    } else {
        throw std::runtime_error(
            "Attempt to set plane camera to non-plane target");
    }
}

static const glm::vec3 cameraUp(0, 1, 0);

std::pair<glm::vec3, glm::vec3>
PlaneCamera::GetFollowingProj(const Entity & target, const Time dt) {
    glm::vec3 cameraTarget = target.GetPosition();
    m_lightView =
        glm::lookAt({cameraTarget.x, cameraTarget.y + 4, cameraTarget.z - 1},
                    cameraTarget, cameraUp);
    glm::vec3 cameraPosition = cameraTarget;
    auto targetRot = target.GetRotation();
    m_currentRotY = MATH::lerp(targetRot.y, m_currentRotY, 0.000002f * dt);
    m_currentRotX = MATH::lerp(targetRot.x, m_currentRotX, 0.000001f * dt);
    m_shiftAmount = MATH::lerp(targetRot.z, m_shiftAmount, 0.00000005f * dt);
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
    m_shiftAmount = MATH::lerp(targetRot.z, m_shiftAmount, 0.000001f * dt);
    cameraPosition.y += 1.4;
    cameraTarget.y += 0.3;
    return {cameraPosition, cameraTarget};
}

void PlaneCamera::Update(const Time dt) {
    if (auto targetSp = m_target.lock()) {
        auto projParams = GetFollowingProj(*targetSp, dt);
        m_position = projParams.first;
        m_cameraView =
            glm::lookAt(projParams.first, projParams.second, cameraUp);
        m_viewDir = projParams.first - projParams.second;
        // if (Singleton<Game>::Instance()
        //         .GetInput()
        //         .buttonSet->AimPressed()) {
        //     m_mode = Mode::TransitionFollowingToShoulder;
        // }
    }
}

void PlaneCamera::DisplayOverlay() {
    // m_reticle.Display();
}

void ElasticCamera::SetPosition(const glm::vec3 & position) {
    m_futurePos = position;
}

void ElasticCamera::Update(const Time dt) {
    if (auto targetSp = m_target.lock()) {
        if (m_position not_eq m_futurePos) {
            static const float LERP_RATE = 0.000005f * dt;
            m_position = MATH::lerp(m_futurePos, m_position, LERP_RATE);
        }
        glm::vec3 cameraTarget = targetSp->GetPosition();
        m_lightView = glm::lookAt(
            {cameraTarget.x, cameraTarget.y + 4, cameraTarget.z - 1},
            cameraTarget, cameraUp);
        m_cameraView = glm::lookAt(m_position, cameraTarget, cameraUp);
    }
}

void ElasticCamera::SetTarget(std::shared_ptr<Entity> target) {
    m_target = target;
    // TODO: Set time constant for elastic snap to new target.
}

const glm::vec3 & Camera::GetViewDir() const { return m_viewDir; }

const glm::mat4 & Camera::GetWorldView() const { return m_cameraView; }

const glm::mat4 & Camera::GetLightView() const { return m_lightView; }

const glm::vec3 & Camera::GetPosition() const { return m_position; }
}
