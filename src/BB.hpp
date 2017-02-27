#pragma once

#include <glm/glm.hpp>

class AABB {
    glm::vec3 m_min, m_max;
public:
    AABB();
    AABB(const glm::vec3 & min, const glm::vec3 & max);
    bool Intersects(const AABB & other);
    void Rotate(const float rad, const glm::vec3 & axis);
    void Translate(const glm::vec3 & translation);
    void Scale(const glm::vec3 scale);
};
