#pragma once

#include "Entity.hpp"
#include "GameMath.hpp"
#include "Time.hpp"
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <SFML/Window.hpp>
#include <cmath>

namespace FLIGHT {
    class Camera {
	std::weak_ptr<Entity> m_target;
	glm::mat4 m_cameraView, m_lightView;
	float m_currentRotY, m_currentRotX, m_shiftAmount;
	glm::vec3 m_viewDir;
    public:
	void Update(const Time dt);
	void SetTarget(std::shared_ptr<Entity> target);
	const glm::mat4 & GetWorldView() const;
	const glm::mat4 & GetLightView() const;
	const glm::vec3 & GetViewDir() const;
    };
}
