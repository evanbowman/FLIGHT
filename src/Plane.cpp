#include "Plane.hpp"

Plane::Plane() : m_pitch{}, m_roll{} {}

void Plane::Display(const GLuint shaderProgram) {
    glm::mat4 modelMatrix;
    modelMatrix = glm::translate(modelMatrix, m_position);
    modelMatrix = glm::rotate(modelMatrix, m_rotation.y, {0, 1, 0});
    modelMatrix = glm::rotate(modelMatrix, m_rotation.z, {0, 0, 1});
    modelMatrix = glm::rotate(modelMatrix, m_rotation.x, {1, 0, 0});
    m_leftWing.Display(modelMatrix, shaderProgram);
    m_rightWing.Display(modelMatrix, shaderProgram);
    m_engine.Display(modelMatrix, shaderProgram);
}

void Plane::Update() {
    // TODO: adjust direction vector based on pitch and yaw
    if (m_roll > 0) {
	m_rotation.y += 0.00001f * m_roll;
    } else {
	m_rotation.y += 0.00001f * m_roll;
    }
}

const glm::vec3 & Plane::GetDirection() const {
    return m_direction;
}

void Plane::SetDirection(const glm::vec3 & direction) {
    m_direction = direction;
}

void Plane::SetPitch(const float pitch) {
    m_pitch = pitch;
}

float Plane::GetPitch() const {
    return m_pitch;
}

void Plane::SetRoll(const float roll) {
    m_roll = roll;
}

float Plane::GetRoll() const {
    return m_roll;
}
