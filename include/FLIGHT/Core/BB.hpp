#pragma once

#include <array>
#include <glm/glm.hpp>

namespace FLIGHT {
class AABB { // A.K.A. Axis Aligned Bounding Box
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
};

class MBS { // A.K.A. Minimum Bounding Sphere
    glm::vec3 m_center;
    float m_radius;

public:
    MBS(const float radius, const glm::vec3 & center);
    MBS(const AABB & aabb);
    bool Intersects(const MBS & other);
    const glm::vec3 & GetCenter() const;
    float GetRadius() const;
};

class OBB { // A.K.A. Oriented Bounding Box
    glm::mat3 m_rotation;
    glm::vec3 m_min, m_max;
    bool ContainsImpl(const glm::vec3 & point, const glm::mat3 & invRot) const;

public:
    OBB() {}
    OBB(const AABB & aabb);
    void Translate(const glm::vec3 & translation);
    void Rotate(const float rad, const glm::vec3 & axis);
    void Scale(const glm::vec3 & scale);
    bool Intersects(const OBB & other) const;
    bool Contains(const glm::vec3 & point) const;
    glm::vec3 GetCenter() const;
    float GetHeight() const;
};
}
