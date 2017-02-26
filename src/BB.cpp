#include "BB.hpp"

AABB::AABB() : m_min{}, m_max{} {}

AABB::AABB(const glm::vec3 & min, const glm::vec3 & max) : m_min(min), m_max(max) {}
