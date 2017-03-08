#include "BB.hpp"
#include "Game.hpp"

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
    // clang-format off
    const float rotX[] = {
        1, 0, 0,
        0, std::cos(rad), -std::sin(rad),
        0, std::sin(rad), std::cos(rad)
    };
    // clang-format on
    auto rot = glm::make_mat3(rotX);
    for (auto & point : cubePoints) {
        point = rot * point;
    }
}

void AABB::DoYRotation(std::array<glm::vec3, 8> & cubePoints, const float rad) {
    // clang-format off
    const float rotY[] {
        std::cos(rad), 0, std::sin(rad),
        0, 1, 0,
        -std::sin(rad), 0, std::cos(rad)
    };
    // clang-format on
    auto rot = glm::make_mat3(rotY);
    for (auto & point : cubePoints) {
        point = rot * point;
    }
}

void AABB::DoZRotation(std::array<glm::vec3, 8> & cubePoints, const float rad) {
    // clang-format off
    const float rotZ[] {
        std::cos(rad), -std::sin(rad), 0,
        std::sin(rad), std::cos(rad), 0,
        0, 0, 1
    };
    // clang-format on
    auto rot = glm::make_mat3(rotZ);
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
}
