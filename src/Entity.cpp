#include "Entity.hpp"

Entity::Entity() : m_position{},
		   m_rotationVec{1, 1, 1},
		   m_rotationAngle(0) {}

void Entity::SetPosition(const glm::vec3 & position) {
    m_position = position;
}

void Entity::SetRotation(const float angle,
			 const glm::vec3 & vec) {
    m_rotationAngle = angle;
    m_rotationVec = vec;
}

const glm::vec3 & Entity::GetPosition() const {
    return m_position;
}

const glm::vec3 & Entity::GetRotationVec() const {
    return m_rotationVec;
}

float Entity::GetRotationAngle() const {
    return m_rotationAngle;
}
