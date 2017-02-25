#pragma once

#include <glm/glm.hpp>

class Reticle {
    glm::vec3 m_position;
public:
    void Update(const Player & player);
};
