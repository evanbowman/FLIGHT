#include <FLIGHT/Core/Game.hpp>
#include <FLIGHT/Scene/Scene.hpp>

namespace FLIGHT {
static inline void RequestChunkNearestPlayersPlane() {
    auto & game = Singleton<Game>::Instance();
    auto playerPlane = game.GetPlayer1().GetPlane();
    const float displ =
        TerrainChunk::vertSpacing * TerrainChunk::GetSidelength();
    auto & pos = playerPlane->GetPosition();
    game.GetTerrainMgr().RequestChunk(std::floor(pos.x / displ),
                                      std::floor(pos.z / displ));
}

WorldLoader::WorldLoader()
    : m_active(true), m_terrainThread([this] {
          RequestChunkNearestPlayersPlane();
          try {
              while (this->m_active) {
                  auto & game = Singleton<Game>::Instance();
                  if (game.GetTerrainMgr().HasWork()) {
                      game.GetTerrainMgr().UpdateTerrainGen();
                  } else {
                      std::this_thread::sleep_for(std::chrono::milliseconds(2));
                  }
              }
          } catch (const std::exception &) {
              auto & game = Singleton<Game>::Instance();
              game.NotifyThreadExceptionOccurred(std::current_exception());
          }
      }) {}

void WorldLoader::UpdateLogic(const Time dt) {
    auto & game = Singleton<Game>::Instance();
    auto & camera = game.GetCamera();
    camera.Update(dt);
    const auto view = camera.GetWorldView();
    auto invView = glm::inverse(view);
    glm::vec3 eyePos = invView * glm::vec4(0, 0, 0, 1);
    auto & terrain = game.GetTerrainMgr();
    terrain.UpdateChunkLOD(eyePos, camera.GetViewDir());
}

void WorldLoader::UpdateState(SceneStack & state) {
    if (Singleton<Game>::Instance().GetTerrainMgr().HasWork()) {
        state.push(std::make_shared<WorldTransitionIn>());
    }
}

void WorldLoader::Display(DisplayImpl & renderer) { renderer.Dispatch(*this); }
}
