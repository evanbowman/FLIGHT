#pragma once

#include <FLIGHT/Entity/Entity.hpp>
#include <FLIGHT/Entity/Plane.hpp>
#include <FLIGHT/Util/Time.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

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
    const glm::vec3 & GetPosition() const;
    const glm::mat4 & GetWorldView() const;
    const glm::mat4 & GetLightView() const;
    const glm::vec3 & GetViewDir() const;
};

class PlaneCamera : public Camera {
    std::weak_ptr<Plane> m_target;
    float m_yOff;
    float m_currentRotY, m_currentRotX, m_shiftAmount;
    Time m_transitionTimer = 0;
    std::pair<glm::vec3, glm::vec3> GetFollowingProj(const Entity & target,
                                                     const Time dt);
public:
    void Update(const Time dt) override;
    void DisplayOverlay() override;
    void SetTarget(std::shared_ptr<Entity> target) override;
};

class ElasticCamera : public Camera {
    std::weak_ptr<Entity> m_target;
    glm::vec3 m_position;
    glm::vec3 m_futurePos;

public:
    ElasticCamera(const glm::vec3 position) : m_position(position), m_futurePos(position) {}
    void SetPosition(const glm::vec3 & position);
    void Update(const Time dt) override;
    void DisplayOverlay() override {}
    void SetTarget(std::shared_ptr<Entity> target) override;
};
}
