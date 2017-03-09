#pragma once

#include "Player.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

namespace FLIGHT {
class Reticle {
    glm::vec3 m_position{};
    void DisplayImpl(ShaderProgram & shader);

public:
    void Update(const Player & player);
    void Display();
};
}
