#include "Plane.hpp"

#include  <cassert>

Plane::Plane() : m_pitch{}, m_roll{}, m_thrust{} {}

void AssertGLStatus(const std::string & context);

void Plane::Display(const GLuint shaderProgram) {
    glm::mat4 modelMatrix;
    modelMatrix = glm::translate(modelMatrix, m_position);
    modelMatrix = glm::rotate(modelMatrix, m_rotation.y, {0, 1, 0});
    modelMatrix = glm::rotate(modelMatrix, m_rotation.z, {0, 0, 1});
    modelMatrix = glm::rotate(modelMatrix, m_rotation.x, {1, 0, 0});
    for (auto & comp : m_components) {
	comp.Display(modelMatrix, shaderProgram);
    }
}

void Plane::SetThrust(const float thrust) {
    m_thrust = thrust;
}

float Plane::GetThrust() const {
    return m_thrust;
}

void Plane::Update(const long long dt) {
    const float rateFactor = 0.00001f * dt;
    static const float yCeil = 32.f;
    static const float yFloor = -3.f;
    const float yDisp = std::sin(m_rotation.x) * rateFactor;
    if (m_position.y + yDisp < yCeil && m_position.y + yDisp > yFloor) {
	m_position.y += yDisp;
    }
    m_position.z -= std::cos(m_rotation.y) * rateFactor;
    m_position.x -= std::sin(m_rotation.y) * rateFactor;
    static const float turningRate = 0.00000002f;
    m_rotation.y += turningRate * dt * m_roll;
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
