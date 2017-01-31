#pragma once

#include "Entity.hpp"
#include "Sprite.hpp"
#include "AssetManager.hpp"

class Plane : public Entity {
protected:
    Sprite m_leftWing, m_rightWing, m_engine;
    glm::vec3 m_direction;
    float m_pitch;
    float m_roll;
public:
    Plane();
    virtual void Display(const GLuint shaderProgram) override;
    void Update();
    const glm::vec3 & GetDirection() const;
    void SetDirection(const glm::vec3 & direction);
    void SetPitch(const float);
    float GetPitch() const;
    void SetRoll(const float);
    float GetRoll() const;
};
