#include "Camera.hpp"
#include "Game.hpp"

namespace FLIGHT {
void Camera::SetTarget(std::shared_ptr<Entity> target) {
    m_target = target;
    m_currentRotY = target->GetRotation().y;
    m_currentRotX = target->GetRotation().x;
    m_shiftAmount = 0.f;
}

static const glm::vec3 cameraUp(0, 1, 0);

    std::pair<glm::vec3, glm::vec3> Camera::GetFollowingProj(const Entity & target, const Time dt) {
    glm::vec3 cameraTarget = target.GetPosition();
    m_lightView = glm::lookAt(
	{cameraTarget.x, cameraTarget.y + 4, cameraTarget.z - 1},
	cameraTarget, cameraUp);
    glm::vec3 cameraPosition = cameraTarget;
    auto targetRot = target.GetRotation();
    m_currentRotY = MATH::lerp(targetRot.y, m_currentRotY, 0.000002f * dt);
    m_currentRotX = MATH::lerp(targetRot.x, m_currentRotX, 0.000001f * dt);
    m_shiftAmount =
	MATH::lerp(targetRot.z, m_shiftAmount, 0.00000005f * dt);
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

    std::pair<glm::vec3, glm::vec3> Camera::GetOverTheShoulderProj(const Entity & target, const Time dt) {
    auto cameraPosition = target.GetPosition();
    m_reticle.Update(target);
    auto fwdDir = target.GetForwardVec();
    glm::vec3 cameraTarget = cameraPosition + fwdDir * 2.5f;
    cameraPosition.y += 0.5;
    cameraPosition.z -= 0.5 * fwdDir.z;
    cameraPosition.x -= 0.5 * fwdDir.x;
    auto targetRot = target.GetRotation();
    m_lightView = glm::lookAt(
	{cameraTarget.x, cameraTarget.y + 4, cameraTarget.z - 1},
	cameraTarget, cameraUp);
    m_cameraView = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
    return {cameraPosition, cameraTarget};
}
    
void Camera::Update(const Time dt) {
    if (auto targetSp = m_target.lock()) {
	switch (m_mode) {
	case Mode::ThirdPersonFollowing: {
	    auto projParams = GetFollowingProj(*targetSp, dt);
	    m_cameraView = glm::lookAt(projParams.first, projParams.second, cameraUp);
	    m_viewDir = projParams.first - projParams.second;
	    if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
		m_mode = Mode::TransitionFollowingToShoulder;
	    }
	} break;

	case Mode::TransitionFollowingToShoulder: {
	    static const Time SWAP_TIME = 200000;
	    if ((m_transitionTimer += dt) < SWAP_TIME) {
		auto fProjParams = GetFollowingProj(*targetSp, dt);
		auto sProjParams = GetOverTheShoulderProj(*targetSp, dt);
		float transitionAmt =
		    glm::smoothstep(0.f, 1.f, (float)m_transitionTimer / SWAP_TIME);
		decltype(fProjParams) projParams {
		    glm::mix(fProjParams.first, sProjParams.first, transitionAmt),
		    glm::mix(fProjParams.second, sProjParams.second, transitionAmt)
		};
		m_cameraView = glm::lookAt(projParams.first, projParams.second, cameraUp);
		m_viewDir = projParams.first - projParams.second;
	    } else {
		m_transitionTimer = 0;
		m_mode = Mode::ThirdPersonOverTheShoulder;
	    }
	} break;

	case Mode::TransitionShoulderToFollowing: {
	    static const Time SWAP_TIME = 300000;
	    if ((m_transitionTimer += dt) < SWAP_TIME) {
		auto fProjParams = GetFollowingProj(*targetSp, dt);
		auto sProjParams = GetOverTheShoulderProj(*targetSp, dt);
		float transitionAmt =
		    glm::smoothstep(0.f, 1.f, (float)m_transitionTimer / SWAP_TIME);
		decltype(fProjParams) projParams {
		    glm::mix(sProjParams.first, fProjParams.first, transitionAmt),
		    glm::mix(sProjParams.second, fProjParams.second, transitionAmt)
		};
		m_cameraView = glm::lookAt(projParams.first, projParams.second, cameraUp);
		m_viewDir = projParams.first - projParams.second;
	    } else {
		m_transitionTimer = 0;
		m_mode = Mode::ThirdPersonFollowing;
	    }
	} break;
	    
	case Mode::ThirdPersonOverTheShoulder: {
	    auto projParams = GetOverTheShoulderProj(*targetSp, dt);
	    m_cameraView = glm::lookAt(projParams.first, projParams.second, cameraUp);
	    m_viewDir = projParams.first - projParams.second;
	    auto targetRot = targetSp->GetRotation();
	    m_currentRotY = targetRot.y;
	    m_currentRotX = targetRot.x;
	    m_shiftAmount = targetRot.z;
	    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
		m_mode = Mode::TransitionShoulderToFollowing;
	    }
	} break;
	}
    }
}

    void Camera::DisplayOverlay() {
        if (m_mode != Mode::ThirdPersonFollowing) {
	    m_reticle.Display();
	}
    }
    
const glm::vec3 & Camera::GetViewDir() const { return m_viewDir; }

const glm::mat4 & Camera::GetWorldView() const { return m_cameraView; }

const glm::mat4 & Camera::GetLightView() const { return m_lightView; }
}
