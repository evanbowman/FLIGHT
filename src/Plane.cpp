#include "Plane.hpp"
#include "CollisionManager.hpp"

Plane::Plane() : m_pitch{}, m_roll{}, m_thrust{}, m_yVelocity{} {}

void AssertGLStatus(const std::string & context);

void Plane::Display(ShaderProgram & shader) {
    glm::mat4 modelMatrix;
    modelMatrix = glm::translate(modelMatrix, m_position);
    modelMatrix = glm::rotate(modelMatrix, m_rotation.y, {0, 1, 0});
    modelMatrix = glm::rotate(modelMatrix, m_rotation.z, {0, 0, 1});
    modelMatrix = glm::rotate(modelMatrix, m_rotation.x, {1, 0, 0});
    for (auto & comp : m_components) {
	comp.Display(modelMatrix, shader);
    }
}

AABB Plane::GetAABB() {
    std::vector<AABB> aabbs;
    for (auto & comp : m_components) {
	aabbs.push_back(comp.GetAABB());
    }
    // ...
    return {};
}

void Plane::SetThrust(const float thrust) {
    m_thrust = thrust;
}

float Plane::GetThrust() const {
    return m_thrust;
}

void Plane::Update(const Time dt) {
    const float rateFactor = 0.000035f * dt;
    static const float yCeil = GetElevationLimit();
    static const float yFloor = -3.f;
    const float yDisp = std::sin(m_rotation.x) * rateFactor;
    m_yVelocity = math::lerp(yDisp, m_yVelocity, 0.05 * dt * 0.0001f);
    if (m_position.y + m_yVelocity < yCeil && m_position.y + m_yVelocity > yFloor) {
	m_position.y += m_yVelocity;
    }
    m_position.z -= std::cos(m_rotation.y) * rateFactor * std::cos(m_rotation.x);
    m_position.x -= std::sin(m_rotation.y) * rateFactor * std::cos(m_rotation.x);
    static const float turningRate = 0.000000025f;
    m_rotation.y += turningRate * dt * m_roll;
}

const glm::vec3 & Plane::GetDirection() const {
    return m_direction;
}

void Plane::SetDirection(const glm::vec3 & direction) {
    m_direction = direction;
}

void Plane::SetPitch(const float pitch) {
    m_rotation.x = glm::radians(pitch);
    m_pitch = pitch;
}

float Plane::GetPitch() const {
    return m_pitch;
}

void Plane::SetRoll(const float roll) {
    m_rotation.z = glm::radians(roll);
    m_roll = roll;
}

float Plane::GetRoll() const {
    return m_roll;
}

void Plane::OnCollide(Solid & other) {
    if (dynamic_cast<Plane *>(&other)) {
	std::cout << "Ran into another plane" << std::endl;
    } else if (dynamic_cast<TerrainCollider *>(&other)) {
	std::cout << "Hit terrain" << std::endl;
    }
}
