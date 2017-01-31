#include "Entity.hpp"

Entity::Entity() : m_position{},
		   m_rotation{0, 0.0000000001f, 0} {}

void Entity::SetPosition(const glm::vec3 & position) {
    m_position = position;
}

void Entity::SetRotation(const glm::vec3 & vec) {
    m_rotation = vec;
}

const glm::vec3 & Entity::GetPosition() const {
    return m_position;
}

const glm::vec3 & Entity::GetRotation() const {
    return m_rotation;
}
