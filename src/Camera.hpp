/// @file
/// @brief Camera view controller interface.

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
/// @class Camera
/// @brief An abstract class that defines a camera interface.
class Camera {
protected:
    glm::vec3 m_viewDir;
    glm::vec3 m_position;
    glm::mat4 m_cameraView, m_lightView;

public:
    /// @brief Display any camera overlay information as needed.
    ///
    /// For example, camera classes that derive from this one may want to
    /// display some sort of reticle.
    virtual void DisplayOverlay() = 0;

    /// @brief Set the camera's target.
    ///
    /// The camera interface defines that it can be set to follow any game
    /// entity. In reality, some cameras, such as PlaneCamera, are more
    /// restrictive about their follow targets.
    /// @param target The target to follow.
    virtual void SetTarget(std::shared_ptr<Entity> target) = 0;

    /// @brief Update the camera.
    /// @param dt Delta time.
    virtual void Update(const Time dt) = 0;

    /// @brief The Camera class' destructor.
    /// Camera is polymorphic.
    virtual ~Camera() {}

    /// @brief Get the center position of the camera.
    /// @return The camera's center position.
    const glm::vec3 & GetPosition() const;

    /// @brief Get the camera's view of the world.
    /// @return A view matrix.
    const glm::mat4 & GetWorldView() const;

    /// @brief Get the view of the world from the perspective of the global
    /// light source based on the camera's center.
    ///
    /// This is used for shadow mapping calculations.
    /// @return A view matrix.
    const glm::mat4 & GetLightView() const;

    /// @brief Get the direction that the camera is pointing.
    /// @return A 3D unit vector representing the Camera's view direction.
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
    /// @brief Update the camera's position to track the plane that it follows.
    /// @param dt Delta time.
    void Update(const Time dt) override;

    /// @brief Display a Reticle if the camera Mode is
    /// Mode::ThirdPersonOverTheShoulder
    void DisplayOverlay() override;

    /// @brief Set the camera target.
    ///
    /// This function throws an exception of the Entity passed to set target
    /// does not derive from Plane
    /// @param The target to follow.
    void SetTarget(std::shared_ptr<Entity> target) override;
};
}
