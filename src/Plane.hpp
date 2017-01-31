#pragma once

#include "Entity.hpp"
#include "Sprite.hpp"
#include "AssetManager.hpp"

class Plane : public Entity {
protected:
    Sprite m_leftWing, m_rightWing, m_engine;
    glm::vec3 m_direction;
public:
    virtual void Display(const GLuint shaderProgram) override;
    const glm::vec3 & GetDirection() const;
    void SetDirection(const glm::vec3 & direction);
};
