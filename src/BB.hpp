/// @file
/// @brief Collision detection primitives

#pragma once

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <noiseutils.h>

#include "GameMath.hpp"
#include "Shader.hpp"

namespace FLIGHT {
/// @class AABB
/// @brief Axis Aligned Bounding Box
///
/// An axis aligned bounding box, represented by it's minimum and maximum
/// extents.
class AABB {
    glm::vec3 m_min, m_max;
    void DoYRotation(std::array<glm::vec3, 8> & cubePoints, const float rad);
    void DoXRotation(std::array<glm::vec3, 8> & cubePoints, const float rad);
    void DoZRotation(std::array<glm::vec3, 8> & cubePoints, const float rad);

public:
    AABB();

    /// @brief Get the box's min extent.
    /// @return glm::vec3
    const glm::vec3 & GetMin() const;

    /// @brief Get the box's max extent.
    /// @return glm::vec3
    const glm::vec3 & GetMax() const;

    /// @brief Construct an AABB from max and min extents
    /// @param min The minimum extent.
    /// @param max The maximum extent.
    AABB(const glm::vec3 & min, const glm::vec3 & max);

    /// @brief Merge with another AABB
    ///
    /// Merge with another AABB such that the box's extents now encompass itself
    /// as well as the other box.
    /// @param other The AABB to merge with.
    void Merge(const AABB & other);

    /// @brief Test intersection with another bounding AABB
    /// @param other The AABB to test intersection with.
    /// @return True if intersection, false otherwise.
    bool Intersects(const AABB & other);

    /// @brief Rotate an AABB
    /// @param rad The number of radians to rotate the box.
    /// @param axis Which axes to rotate around.
    void Rotate(const float rad, const glm::vec3 & axis);

    /// @brief Translate an AABB
    /// @param translation How far to move the box.
    void Translate(const glm::vec3 & translation);

    /// @brief Scale an AABB
    /// @param scale Scale vector.
    void Scale(const glm::vec3 & scale);

    /// @brief Display an AABB
    /// @param shader The program by which to draw the box.
    void Display(ShaderProgram & shader);
};

/// @class MBS
/// @brief Minumum Bounding Sphere
///
/// A collision detection primitive
class MBS {
    glm::vec3 m_center;
    float m_radius;

public:
    /// @brief Construct a MBS from center and radius info
    /// @param radius Radius of the MBS.
    /// @param center Center of the MBS.
    MBS(const float radius, const glm::vec3 & center);

    /// @brief Construct an MBS from an AABB
    /// @param aabb The box to construct the boundind sphere from.
    MBS(const AABB & aabb);

    /// @brief Test intersection with another minimum bounding sphere.
    /// @param other The other sphere to test intersection with.
    /// @return true if an intersection exists, otherwise false.
    bool Intersects(const MBS & other);

    /// @brief Get the sphere's center.
    /// @return A vector representing the center of the sphere.
    const glm::vec3 & GetCenter() const;

    /// @brief Get the sphere's radius.
    /// @return The radius of the sphere.
    float GetRadius() const;
};

/// @class OBB
/// @brief Oriented Bounding Box
///
/// Similar to an AABB, but it stores its own local rotation matrix, making
/// rotate operations cheaper.
class OBB {
    glm::mat3 m_rotation;
    glm::vec3 m_min, m_max;
    bool ContainsImpl(const glm::vec3 & point, const glm::mat3 & invRot) const;

public:
    OBB() {}

    /// @brief Construct an OBB from an existing AABB
    /// @param aabb The AABB to construct the OBB from.
    OBB(const AABB & aabb);

    /// @brief Rotate an AABB
    /// @param rad The number of radians to rotate the box.
    /// @param axis Which axes to rotate around.
    void Rotate(const float rad, const glm::vec3 & axis);

    /// @brief Translate an AABB
    /// @param translation How far to move the box.
    void Translate(const glm::vec3 & translation);

    /// @brief Scale an AABB
    /// @param scale Scale vector.
    void Scale(const glm::vec3 & scale);

    /// @brief Test intersection with another OBB
    /// @param other The other volume to test colision with.
    /// @return true if an intersection exists, otherwise false.
    bool Intersects(const OBB & other) const;

    /// @brief Test whether the volume contains a point
    /// @param point The point to test.
    /// @return true if the point falls within the box, otherwise false.
    bool Contains(const glm::vec3 & point) const;

    /// @brief Get the center point of the box
    /// @return A vector representing the center of the box.
    glm::vec3 GetCenter() const;

    /// @brief Get the height of the box
    /// @return The height of the box.
    float GetHeight() const;
};
}
