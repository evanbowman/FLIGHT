#include "Reticle.hpp"
#include "Game.hpp"
#include <iostream>

namespace FLIGHT {
void Reticle::Update(const Entity & projectFrom) {
    m_position = projectFrom.GetPosition();
    glm::vec3 dir = projectFrom.GetForwardVec();
    static const float RETICLE_DIST_FROM_CENTROID(400.f);
    m_position += RETICLE_DIST_FROM_CENTROID * dir;
    auto windowSize = GetGame().GetWindowSize();
    glm::mat4 view = GetGame().GetCamera().GetWorldView();
    glm::mat4 proj =
        glm::perspective(45.0f, windowSize.x / (float)windowSize.y, 0.1f, 1.0f);
    glm::vec4 viewPort = {0, 0, windowSize.x, windowSize.y};
    m_position = glm::project(m_position, view, proj, viewPort);
    m_position.z = 0.f;
}

void Reticle::DisplayImpl(ShaderProgram & shader) {
    shader.Use();
    PRIMITIVES::TexturedQuad quad;
    glm::mat4 model = glm::translate(glm::mat4(1), m_position);
    model = glm::scale(model, {20.f, 20.f, 0.f});
    shader.SetUniformMat4("model", model);
    quad.Display(shader,
                 {BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha});
}

void Reticle::Display() {
    auto & reticleProg =
        GetGame().GetAssetMgr().GetProgram<ShaderProgramId::Reticle>();
    auto & reticleShadowProg =
        GetGame().GetAssetMgr().GetProgram<ShaderProgramId::ReticleShadow>();
    DisplayImpl(reticleShadowProg);
    DisplayImpl(reticleProg);
}
}
