#include "../Scene.hpp"
#include "../Game.hpp"

WorldLoader::WorldLoader() : m_active(true), m_terrainThread([this] {
    while (m_active) {
	auto & game = GetGame();
	if (game.GetTerrain().HasWork()) {
	    game.GetTerrain().UpdateTerrainGen(); 
	} else {
	    std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}
    }
 }) {
    // In the future, the starting plane should be set by a scene further
    // up the Pipeline
    auto startPlane = std::make_shared<RedTail>();
    startPlane->SetPosition({15.f, 30.f, 15.f});
    GetGame().GetPlayer().GivePlane(startPlane);
    GetGame().GetCamera().SetTarget(startPlane);
}

void WorldLoader::UpdateLogic(const Time dt) {
    auto & camera = GetGame().GetCamera();
    camera.Update(dt);
    const auto view = camera.GetWorldView();
    auto invView = glm::inverse(view);
    glm::vec3 eyePos = invView * glm::vec4(0, 0, 0, 1);
    auto & terrain = GetGame().GetTerrain();
    terrain.UpdateChunkLOD(eyePos, camera.GetViewDir());
}

void WorldLoader::UpdateState(SceneStack & state) {
    if (!GetGame().GetTerrain().HasWork()) {
	state.push(std::make_unique<WorldTransitionIn>());
    }
}

void WorldLoader::Display() {
    GetGame().GetTerrain().SwapChunks();
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
}
