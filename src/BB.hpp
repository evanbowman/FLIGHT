#pragma once

#include <glm/glm.hpp>

class AABB {
    glm::vec3 m_min, m_max;
public:
    AABB();
    AABB(const glm::vec3 & min, const glm::vec3 & max);
};

class BB {
    glm::vec3 c, r;
};
