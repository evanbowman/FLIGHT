#include "Plane.hpp"

void Plane::Display(const GLuint shaderProgram) {
    glm::mat4 modelMatrix;
    modelMatrix = glm::translate(modelMatrix, m_position);
    modelMatrix = glm::rotate(modelMatrix, m_rotation.x, {1, 0, 0});
    modelMatrix = glm::rotate(modelMatrix, m_rotation.y, {0, 1, 0});
    modelMatrix = glm::rotate(modelMatrix, m_rotation.z, {0, 0, 1});
    m_leftWing.Display(modelMatrix, shaderProgram);
    m_rightWing.Display(modelMatrix, shaderProgram);
    m_engine.Display(modelMatrix, shaderProgram);
}

const glm::vec3 & Plane::GetDirection() const {
    return m_direction;
}

void Plane::SetDirection(const glm::vec3 & direction) {
    m_direction = direction;
}
