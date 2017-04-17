#include <FLIGHT/Core/Game.hpp>
#include <FLIGHT/Scene/Scene.hpp>

namespace FLIGHT {
std::mutex g_updateMtx;

World::World() {}

void World::UpdateLogic(const Time dt) {
    auto & game = Singleton<Game>::Instance();
    auto & camera = game.GetCamera();
    game.GetCollisionMgr().Update();
    {
        std::lock_guard<std::mutex> lk(g_updateMtx);
        game.GetPlayer1().Update(dt);
        game.UpdateEntities(dt);
        camera.Update(dt);
    }
    const auto view = camera.GetWorldView();
    auto invView = glm::inverse(view);
    glm::vec3 eyePos = invView * glm::vec4(0, 0, 0, 1);
    game.GetTerrainMgr().UpdateChunkLOD(eyePos, camera.GetViewDir());
    game.GetSkyMgr().Update(eyePos, camera.GetViewDir());
}

void World::UpdateState(SceneStack &) {
    // ... TODO ...
}

void World::Display(DisplayImpl & renderer) {
    std::lock_guard<std::mutex> lk(g_updateMtx);
    renderer.Dispatch(*this);
}
}
