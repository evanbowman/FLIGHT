#include "Plane.hpp"

void Plane::Display(const GLuint shaderProgram) {
    glm::mat4 modelMatrix;
    modelMatrix = glm::rotate(modelMatrix, m_rotationAngle, m_rotationVec);
    modelMatrix = glm::translate(modelMatrix, m_position);
    m_leftWing.Display(modelMatrix, shaderProgram);
    m_rightWing.Display(modelMatrix, shaderProgram);
    m_engine.Display(modelMatrix, shaderProgram);
}
