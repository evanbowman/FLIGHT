#pragma once

#include "Entity.hpp"
#include "GameMath.hpp"
#include "Plane.hpp"
#include "Time.hpp"
#include <SFML/Window.hpp>
#include <chrono>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "Reticle.hpp"

namespace FLIGHT {
class Camera {
protected:
    glm::vec3 m_viewDir;
    glm::vec3 m_position;
    glm::mat4 m_cameraView, m_lightView;

public:
    virtual void DisplayOverlay() = 0;
    virtual void SetTarget(std::shared_ptr<Entity> target) = 0;
    virtual void Update(const Time dt) = 0;
    virtual ~Camera() {}
    void Shake(const float intensity, const float attenuation);
    const glm::vec3 & GetPosition() const;
    const glm::mat4 & GetWorldView() const;
    const glm::mat4 & GetLightView() const;
    const glm::vec3 & GetViewDir() const;
};

class PlaneCamera : public Camera {
    enum class Mode {
        ThirdPersonFollowing,
        TransitionFollowingToShoulder,
        TransitionShoulderToFollowing,
        ThirdPersonOverTheShoulder
    };
    Mode m_mode = Mode::ThirdPersonFollowing;
    std::weak_ptr<Plane> m_target;
    float m_yOff;
    float m_currentRotY, m_currentRotX, m_shiftAmount;
    Reticle m_reticle;
    Time m_transitionTimer = 0;
    std::pair<glm::vec3, glm::vec3> GetFollowingProj(const Entity & target,
                                                     const Time dt);
    std::pair<glm::vec3, glm::vec3>
    GetOverTheShoulderProj(const Entity & target, const Time dt);

public:
    void Update(const Time dt) override;
    void DisplayOverlay() override;
    void SetTarget(std::shared_ptr<Entity> target) override;
};
}
