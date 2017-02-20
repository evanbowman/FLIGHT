#pragma once

#include "Entity.hpp"
#include "Sprite.hpp"
#include "AssetManager.hpp"
#include "Time.hpp"

class Plane : public Entity {
protected:
    std::vector<Sprite> m_components;
    glm::vec3 m_direction;
    float m_pitch;
    float m_roll;
    float m_thrust;
public:
    constexpr static float GetElevationLimit() {
	return 70.f;
    }
    Plane();
    void Display(const GLuint shaderProgram) override;
    void Update(const Time dt);
    const glm::vec3 & GetDirection() const;
    void SetDirection(const glm::vec3 & direction);
    void SetPitch(const float);
    float GetPitch() const;
    void SetRoll(const float);
    float GetRoll() const;
    void SetThrust(const float);
    float GetThrust() const;
};
