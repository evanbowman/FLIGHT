#include "Scene.hpp"
#include "Game.hpp"

WorldLoader::WorldLoader() : m_active(true), m_terrainThread([this] {
    while (m_active) {
	if (GetGame().GetTerrain().HasWork()) {
	    GetGame().GetTerrain().UpdateTerrainGen(); 
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

void WorldLoader::Update(const long long dt) {
    auto & camera = GetGame().GetCamera();
    camera.Update(dt);
    const auto view = camera.GetWorldView();
    auto invView = glm::inverse(view);
    glm::vec3 eyePos = invView * glm::vec4(0, 0, 0, 1);
    auto & terrain = GetGame().GetTerrain();
    terrain.UpdateChunkLOD(eyePos, camera.GetViewDir());
    if (!terrain.HasWork()) {
	GetGame().PushScene(std::make_unique<World>());
    }
}

void WorldLoader::Display() {
    GetGame().GetTerrain().SwapChunks();
}

void WorldLoader::TransitionIn(const float) {}

void WorldLoader::TransitionOut(const float) {}


World::World() {
    
}

void World::Update(const long long dt) {
    auto & camera = GetGame().GetCamera();
    {
	std::lock_guard<std::mutex> lk(m_updateMtx);
	GetGame().GetPlayer().Update(dt);
	camera.Update(dt);
    }
    const auto view = camera.GetWorldView();
    auto invView = glm::inverse(view);
    glm::vec3 eyePos = invView * glm::vec4(0, 0, 0, 1);
    GetGame().GetTerrain().UpdateChunkLOD(eyePos, camera.GetViewDir());
}

void World::Display() {
    auto & game = GetGame();
    std::lock_guard<std::mutex> lk(m_updateMtx);
    game.UpdateProjectionUniforms();
    game.GetTerrain().SwapChunks();
    game.DrawShadowMap();
    const auto & windowSize = game.GetWindowSize();
    glViewport(0, 0, windowSize.x, windowSize.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    game.DrawTerrain();
    const GLuint lightingProg = game.GetAssets().GetShaderProgram(ShaderProgramId::Base);
    glUseProgram(lightingProg);
    const auto view = game.GetCamera().GetWorldView();
    auto invView = glm::inverse(view);
    glm::vec3 eyePos = invView * glm::vec4(0, 0, 0, 1);
    const GLint eyePosLoc = glGetUniformLocation(lightingProg, "eyePos");
    glUniform3f(eyePosLoc, eyePos[0], eyePos[1], eyePos[2]);
    glUniform1i(glGetUniformLocation(lightingProg, "shadowMap"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, game.GetShadowMapTxtr());
    game.GetPlayer().GetPlane()->Display(lightingProg);
}

void World::TransitionIn(const float) {}

void World::TransitionOut(const float) {}
