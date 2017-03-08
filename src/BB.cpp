#include "BB.hpp"
#include "Game.hpp"

#include <glm/gtx/string_cast.hpp>

namespace FLIGHT {
AABB::AABB() : m_min{}, m_max{} {}

AABB::AABB(const glm::vec3 & min, const glm::vec3 & max)
    : m_min(min), m_max(max) {}

bool AABB::Intersects(const AABB & other) {
    return m_min.x <= other.m_max.x && m_max.x >= other.m_min.x &&
           m_min.y <= other.m_max.y && m_max.y >= other.m_min.y &&
           m_min.z <= other.m_max.z && m_max.z >= other.m_min.z;
}

void AABB::Translate(const glm::vec3 & translation) {
    m_min += translation;
    m_max += translation;
}

void AABB::DoXRotation(std::array<glm::vec3, 8> & cubePoints, const float rad) {
    auto rot = MATH::MakeXRotMat(rad);
    for (auto & point : cubePoints) {
        point = rot * point;
    }
}

void AABB::DoYRotation(std::array<glm::vec3, 8> & cubePoints, const float rad) {
    auto rot = MATH::MakeYRotMat(rad);
    for (auto & point : cubePoints) {
        point = rot * point;
    }
}

void AABB::DoZRotation(std::array<glm::vec3, 8> & cubePoints, const float rad) {
    auto rot = MATH::MakeZRotMat(rad);
    for (auto & point : cubePoints) {
        point = rot * point;
    }
}

void AABB::Rotate(const float rad, const glm::vec3 & axis) {
    std::array<glm::vec3, 8> cubeCorners{m_min,
                                         m_max,
                                         {m_max.x, m_max.y, m_min.z},
                                         {m_min.x, m_max.y, m_max.z},
                                         {m_min.x, m_max.y, m_min.z},
                                         {m_max.x, m_min.y, m_min.z},
                                         {m_min.x, m_min.y, m_max.z},
                                         {m_max.x, m_min.y, m_max.z}};
    if (axis.x != 0.f) {
        DoXRotation(cubeCorners, rad);
    }
    if (axis.y != 0.f) {
        DoYRotation(cubeCorners, rad);
    }
    if (axis.z != 0.f) {
        DoZRotation(cubeCorners, rad);
    }
    auto min = cubeCorners[0];
    auto max = cubeCorners[0];
    for (int i = 1; i < cubeCorners.size(); ++i) {
        min.x = std::min(cubeCorners[i].x, min.x);
        min.y = std::min(cubeCorners[i].y, min.y);
        min.z = std::min(cubeCorners[i].z, min.z);
        max.x = std::max(cubeCorners[i].x, max.x);
        max.y = std::max(cubeCorners[i].y, max.y);
        max.z = std::max(cubeCorners[i].z, max.z);
    }
    m_max = max;
    m_min = min;
}

void AABB::Scale(const glm::vec3 & scale) {
    m_min *= scale;
    m_max *= scale;
}

void AABB::Merge(const AABB & other) {
    m_min.x = std::min(m_min.x, other.m_min.x);
    m_min.y = std::min(m_min.y, other.m_min.y);
    m_min.z = std::min(m_min.z, other.m_min.z);
    m_max.x = std::max(m_max.x, other.m_max.x);
    m_max.y = std::max(m_max.y, other.m_max.y);
    m_max.z = std::max(m_max.z, other.m_max.z);
}

const glm::vec3 & AABB::GetMin() const { return m_min; }

const glm::vec3 & AABB::GetMax() const { return m_max; }

void AABB::Display(ShaderProgram & shader) {
    auto binding =
        GetGame().GetAssetMgr().GetModel<ModelId::Box>()->Bind(shader);
    glm::mat4 model;
    model = glm::translate(model, m_min);
    model = glm::scale(
        model, {m_max.x - m_min.x, m_max.y - m_min.y, m_max.z - m_min.z});
    shader.SetUniformMat4("model", model);
    shader.SetUniformVec4("color", {0.949f, 0.765f, 0.027f, 0.25f});
    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_TRIANGLES, 0, binding.numVertices);
    glCullFace(GL_FRONT);
    glBlendFunc(GL_ONE, GL_ZERO);
}

MBS::MBS(const float radius, const glm::vec3 & center)
    : m_center(center), m_radius(radius) {}

MBS::MBS(const AABB & aabb) {
    const glm::vec3 & aabbMin = aabb.GetMin();
    const glm::vec3 & aabbMax = aabb.GetMax();
    const glm::vec3 center{(aabbMin.x + aabbMax.x) / 2,
                           (aabbMin.y + aabbMax.y) / 2,
                           (aabbMin.z + aabbMax.z) / 2};
    m_center = center;
    m_radius = glm::length(aabbMax - center);
}

float MBS::GetRadius() const { return m_radius; }

const glm::vec3 & MBS::GetCenter() const { return m_center; }

bool MBS::Intersects(const MBS & other) {
    auto centerDist = std::abs(glm::length(m_center - other.GetCenter()));
    if (centerDist < (m_radius + other.GetRadius())) {
        return true;
    }
    return false;
}

void OBB::Translate(const glm::vec3 & translation) {
    m_min += translation;
    m_max += translation;
}

void OBB::Scale(const glm::vec3 & scale) {
    m_min *= scale;
    m_max *= scale;
}

void OBB::Rotate(const float rad, const glm::vec3 & axis) {
    if (axis.x != 0.f) {
        auto rot = MATH::MakeXRotMat(rad);
        m_rotation = rot * m_rotation;
    }
    if (axis.y != 0.f) {
        auto rot = MATH::MakeYRotMat(rad);
        m_rotation = rot * m_rotation;
    }
    if (axis.z != 0.f) {
        auto rot = MATH::MakeZRotMat(rad);
        m_rotation = rot * m_rotation;
    }
}

bool OBB::Intersects(const OBB & other) const {
    // TODO: test based on separating axis theorem
    return false;
}

OBB::OBB(const AABB & aabb) : m_min(aabb.GetMin()), m_max(aabb.GetMax()) {}

bool OBB::Contains(const glm::vec3 & point) const {
    auto center = (m_max + m_min) / 2.f;
    auto pTransform = (glm::inverse(m_rotation) * (point - center)) + center;
    return (pTransform.x <= m_max.x && pTransform.y <= m_max.y &&
            pTransform.z <= m_max.z && pTransform.x >= m_min.x &&
            pTransform.y >= m_min.y && pTransform.z >= m_min.z);
}
}
