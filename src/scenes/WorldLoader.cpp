#include "../Game.hpp"
#include "../Scene.hpp"

WorldLoader::WorldLoader()
    : m_active(true), m_terrainThread([this] {
          try {
              while (this->m_active) {
                  auto & game = GetGame();
                  if (game.GetTerrainMgr().HasWork()) {
                      game.GetTerrainMgr().UpdateTerrainGen();
                  } else {
                      std::this_thread::sleep_for(std::chrono::milliseconds(2));
                  }
              }
          } catch (const std::exception &) {
              GetGame().NotifyThreadExceptionOccurred(std::current_exception());
          }
      }) {
    // In the future, the starting plane should be set by a scene further
    // up the Pipeline
    auto startPlane = GetGame().CreateSolid<RedTail>();
    startPlane->SetPosition({15.f, 40.f, 15.f});
    GetGame().GetPlayer().GivePlane(startPlane);
    GetGame().GetCamera().SetTarget(startPlane);
}

void WorldLoader::UpdateLogic(const Time dt) {
    auto & camera = GetGame().GetCamera();
    camera.Update(dt);
    const auto view = camera.GetWorldView();
    auto invView = glm::inverse(view);
    glm::vec3 eyePos = invView * glm::vec4(0, 0, 0, 1);
    auto & terrain = GetGame().GetTerrainMgr();
    terrain.UpdateChunkLOD(eyePos, camera.GetViewDir());
}

void WorldLoader::UpdateState(SceneStack & state) {
    if (!GetGame().GetTerrainMgr().HasWork()) {
        state.push(std::make_shared<WorldTransitionIn>());
    }
}

bool WorldLoader::Display() {
    GetGame().GetTerrainMgr().SwapChunks();
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    return true;
}
