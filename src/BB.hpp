#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.hpp"

namespace FLIGHT {    
    class AABB {
	glm::vec3 m_min, m_max;
    public:
	AABB();
	const glm::vec3 & GetMin() const;
	const glm::vec3 & GetMax() const;
	AABB(const glm::vec3 & min, const glm::vec3 & max);
	bool Intersects(const AABB & other);
	void Rotate(const float rad, const glm::vec3 & axis);
	void Translate(const glm::vec3 & translation);
	void Scale(const glm::vec3 scale);
	void Display(ShaderProgram & shader);
    };

    class OBB {};
}
