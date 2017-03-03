#include "BB.hpp"

AABB::AABB() : m_min{}, m_max{} {}

AABB::AABB(const glm::vec3 & min, const glm::vec3 & max)
    : m_min(min), m_max(max) {}

bool AABB::Intersects(const AABB & other) {
    return (m_max.x > other.m_max.x && m_min.x < other.m_max.x &&
            m_max.y > other.m_min.y && m_min.y < other.m_max.y &&
            m_max.z > other.m_min.z && m_min.z < other.m_max.z);
}
