#include <FLIGHT/Core/Game.hpp>
#include <FLIGHT/Scene/Scene.hpp>

namespace FLIGHT {
std::mutex g_updateMtx;

World::World() {}

void World::UpdateLogic(const Time dt) {
    auto & camera = GetGame().GetCamera();
    GetGame().GetCollisionMgr().Update();
    {
        std::lock_guard<std::mutex> lk(g_updateMtx);
        GetGame().GetPlayer().Update(dt);
        GetGame().UpdateEntities(dt);
        camera.Update(dt);
    }
    const auto view = camera.GetWorldView();
    auto invView = glm::inverse(view);
    glm::vec3 eyePos = invView * glm::vec4(0, 0, 0, 1);
    GetGame().GetTerrainMgr().UpdateChunkLOD(eyePos, camera.GetViewDir());
    GetGame().GetSkyMgr().Update(eyePos, camera.GetViewDir());
}

void World::UpdateState(SceneStack & state) {
    if (GetGame().GetInput().buttonSet->PausePressed()) {
        state.push(std::make_shared<MenuTransitionIn>());
    }
}

void World::Display(DisplayDispatcher & dispatcher) {
    std::lock_guard<std::mutex> lk(g_updateMtx);
    dispatcher.Dispatch(*this);
}
}
