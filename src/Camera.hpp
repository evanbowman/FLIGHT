#pragma once

#include "Entity.hpp"
#include "GameMath.hpp"
#include "Time.hpp"
#include <SFML/Window.hpp>
#include <chrono>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "Reticle.hpp"

namespace FLIGHT {
class Camera {
    enum class Mode {
        ThirdPersonFollowing,
	TransitionFollowingToShoulder,
	TransitionShoulderToFollowing,
	ThirdPersonOverTheShoulder
    };
    Mode m_mode = Mode::ThirdPersonFollowing;
    std::weak_ptr<Entity> m_target;
    glm::mat4 m_cameraView, m_lightView;
    float m_currentRotY, m_currentRotX, m_shiftAmount;
    glm::vec3 m_viewDir;
    Reticle m_reticle;
    Time m_transitionTimer = 0;
    std::pair<glm::vec3, glm::vec3> GetFollowingProj(const Entity & target, const Time dt);
    std::pair<glm::vec3, glm::vec3> GetOverTheShoulderProj(const Entity & target, const Time dt);
    
public:
    void Update(const Time dt);
    void DisplayOverlay();
    void SetTarget(std::shared_ptr<Entity> target);
    const glm::mat4 & GetWorldView() const;
    const glm::mat4 & GetLightView() const;
    const glm::vec3 & GetViewDir() const;
};
}
