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

const glm::vec3 & AABB::GetMin() const { return m_min; }

const glm::vec3 & AABB::GetMax() const { return m_max; }

void AABB::Display(ShaderProgram & shader) {
    auto binding =
        GetGame().GetAssetMgr().GetModel<ModelId::Box>()->Bind(shader);
    glm::mat4 model;
    model = glm::translate(model, {m_min.x, m_min.y, m_min.z});
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
