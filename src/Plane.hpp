#pragma once

#include "Entity.hpp"
#include "Sprite.hpp"
#include "AssetManager.hpp"

class Plane : public Entity {
protected:
    // Sprite m_leftWing, m_rightWing, m_engine, m_fuselage, m_leftStabilizer, m_rightStabilizer;
    std::vector<Sprite> m_components;
    glm::vec3 m_direction;
    float m_pitch;
    float m_roll;
    float m_thrust;
public:
    Plane();
    void Display(const GLuint shaderProgram) override;
    void Update(const long long dt);
    const glm::vec3 & GetDirection() const;
    void SetDirection(const glm::vec3 & direction);
    void SetPitch(const float);
    float GetPitch() const;
    void SetRoll(const float);
    float GetRoll() const;
    void SetThrust(const float);
    float GetThrust() const;
};
