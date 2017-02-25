#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Player.hpp"

class Reticle {
    glm::vec3 m_position{};
    void DisplayImpl(const GLuint shaderProg);
public:
    void Update(const Player & player);
    void Display();
};
