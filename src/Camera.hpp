#pragma once

#include "Entity.hpp"
#include "GameMath.hpp"
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <SFML/Window.hpp>
#include <cmath>

class Camera {
    std::weak_ptr<Entity> m_target;
    glm::mat4 m_view;
    float m_currentRotY, m_currentRotX, m_shiftAmount;
public:
    void Update(const long long dt);
    void SetTarget(std::shared_ptr<Entity> target);
    glm::mat4 GetView() const;
};
