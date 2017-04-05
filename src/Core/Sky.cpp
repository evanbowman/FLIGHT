#include <FLIGHT/Core/Game.hpp>
#include <FLIGHT/Core/Sky.hpp>

namespace FLIGHT {
std::array<SkyManager::Flare, 11> g_lensFlares{
    {{18.f, 0.9f, {}, 1.f},
     {46.f, 0.45f, {}, 1.f},
     {100.f, 0.6f, {}, 1.f},
     {200.f, 0.4f, {}, 1.f},
     {300.f, 1.2f, {}, 1.f},
     {360.f, 0.15f, {}, 1.f},
     {500.f, 2.5f, {}, 1.f},
     {525.f, 0.4f, {}, 1.f},
     {1350.f, 4.5f, {}, 1.f},
     {1700.f, 3.6f, {}, 1.f},
     {1850.f, 4.8f, {}, 1.f}}};

void SkyManager::Update(const glm::vec3 & cameraPos,
                        const glm::vec3 & viewDir) {
    m_sunPos = cameraPos - glm::vec3{0, 0, 330};
    m_sunPos.y = 180;
    m_skydomeLocus = cameraPos;
    m_sunVisible = IntersectsFrustum(m_sunPos, cameraPos, viewDir);
    m_rot.y = std::atan2f(viewDir.x, viewDir.z);
    m_rot.x = std::atan2f(viewDir.y, viewDir.z);
    if (m_sunVisible) {
        auto windowSize = Singleton<Game>::Instance().GetWindowSize();
        glm::mat4 view = Singleton<Game>::Instance().GetCamera().GetWorldView();
        glm::mat4 proj = glm::perspective(
            45.0f, windowSize.x / (float)windowSize.y, 0.1f, 1.0f);
        glm::vec4 viewPort = {0, 0, windowSize.x, windowSize.y};
        auto sunScreenCoord = glm::project(m_sunPos, view, proj, viewPort);
        // The frustum check can be inexact for far away things (eg the sun),
        // and
        // it's really important here
        // to only draw the lens flare when the sun is visible in the view!
        static const float sunRad = 48.f;
        if (sunScreenCoord.y > windowSize.y + sunRad) {
            m_sunVisible = false;
            return;
        }
        glm::vec3 directionVec = {
            glm::vec3{windowSize.x / 2, windowSize.y / 2, 0.f} -
            glm::vec3{sunScreenCoord.x, sunScreenCoord.y, 0.f}};
        const float dist = glm::length(directionVec);
        directionVec = glm::normalize(directionVec);
        const float distRatioToMax = dist / 900.f;
        for (auto & flare : g_lensFlares) {
            flare.position =
                glm::vec3{sunScreenCoord.x, sunScreenCoord.y, 0.f} +
                glm::vec3(directionVec * flare.displFactor * distRatioToMax);
            flare.intensity = 1.f - distRatioToMax;
        }
    }
}

void SkyManager::Display(DisplayImpl & renderer) {
    renderer.Dispatch(*this);
}
}
