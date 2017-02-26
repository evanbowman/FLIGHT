#pragma once

#include <glm/glm.hpp>
#include <OpenGL/gl3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <memory>

#include "Time.hpp"

class Entity {
protected:
    glm::vec3 m_position;
    glm::vec3 m_rotation;
public:
    void SetPosition(const glm::vec3 & position);
    void SetRotation(const glm::vec3 & vec);
    glm::vec3 GetForwardVec() const;
    const glm::vec3 & GetPosition() const;
    const glm::vec3 & GetRotation() const;
    Entity();
    virtual void Display(const GLuint shaderProgram) = 0;
    virtual void Update(const Time dt) = 0;
    virtual ~Entity() {}
};

class Solid : public Entity {
    // ... maybe: virtual const AABB & GetBounds() const = 0;
};
