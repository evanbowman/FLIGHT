#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <noiseutils.h>
#include <array>

#include "Shader.hpp"

namespace FLIGHT {
    class AABB {
	glm::vec3 m_min, m_max;
	void DoYRotation(std::array<glm::vec3, 8> & cubePoints, const float rad);
	void DoXRotation(std::array<glm::vec3, 8> & cubePoints, const float rad);
	void DoZRotation(std::array<glm::vec3, 8> & cubePoints, const float rad);
    public:
	AABB();
	const glm::vec3 & GetMin() const;
	const glm::vec3 & GetMax() const;
	AABB(const glm::vec3 & min, const glm::vec3 & max);
	void Merge(const AABB & other);
	bool Intersects(const AABB & other);
	void Rotate(const float rad, const glm::vec3 & axis);
	void Translate(const glm::vec3 & translation);
	void Scale(const glm::vec3 & scale);
	void Display(ShaderProgram & shader);
    };

    class MBS {
	glm::vec3 m_center;
	float m_radius;
    public:
	MBS(const float radius, const glm::vec3 & center);
	MBS(const AABB & aabb);
	bool Intersects(const MBS & other);
	const glm::vec3 & GetCenter() const;
	float GetRadius() const;
    };
    
    class OBB {
	std::array<glm::vec3, 3> rotation;
	glm::vec3 center;
	glm::vec3 extents;
    public:
	// ...
    };
}
