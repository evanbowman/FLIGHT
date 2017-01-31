#pragma once

#include <glm/glm.hpp>
#include <OpenGL/gl3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

class App;

class Entity {
protected:
    glm::vec3 m_position;
    glm::vec3 m_rotation;
public:
    void SetPosition(const glm::vec3 & position);
    void SetRotation(const glm::vec3 & vec);
    const glm::vec3 & GetPosition() const;
    const glm::vec3 & GetRotation() const;
    Entity();
    virtual void Display(const GLuint shaderProgram) = 0;
    virtual ~Entity() {}
};
