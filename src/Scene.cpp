#include "Scene.hpp"
#include "Game.hpp"


void TitleScreen::Update(const long long dt) {
    GetGame().PushScene(std::make_unique<WorldLoader>());
}

void TitleScreen::Display() {
    
}


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
    GetGame().GetSky().Update(eyePos, camera.GetViewDir());
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
	GetGame().PushScene(std::make_unique<Menu>());
    }
}

void World::DrawTerrain() {
    const GLuint terrainProg = GetGame().GetAssets().GetShaderProgram(ShaderProgramId::Terrain);
    glUseProgram(terrainProg);
    const auto view = GetGame().GetCamera().GetWorldView();
    auto invView = glm::inverse(view);
    glm::vec3 eyePos = invView * glm::vec4(0, 0, 0, 1);
    const GLint eyePosLoc = glGetUniformLocation(terrainProg, "eyePos");
    glUniform3f(eyePosLoc, eyePos[0], eyePos[1], eyePos[2]);
    GetGame().GetTerrain().Display(terrainProg);
    AssertGLStatus("terrain rendering");
}

void World::DrawSky() {
    GetGame().GetSky().Display();
}

void World::UpdatePerspProjUniforms() {
    auto & assets = GetGame().GetAssets();
    const GLuint shadowProgram = assets.GetShaderProgram(ShaderProgramId::Shadow);
    const GLuint lightingProg = assets.GetShaderProgram(ShaderProgramId::Base);
    const GLuint terrainProg = assets.GetShaderProgram(ShaderProgramId::Terrain);
    const GLuint genericTxtrdProg = assets.GetShaderProgram(ShaderProgramId::GenericTextured);
    glUseProgram(shadowProgram);
    auto & camera = GetGame().GetCamera();
    auto view = camera.GetLightView();
    auto lightSpace = ::LIGHT_PROJ_MAT * view;
    GLint lightSpaceLoc = glGetUniformLocation(shadowProgram, "lightSpace");
    glUniformMatrix4fv(lightSpaceLoc, 1, GL_FALSE, glm::value_ptr(lightSpace));

    glUseProgram(lightingProg);
    view = camera.GetWorldView();
    const auto & windowSize = GetGame().GetWindowSize();
    const float aspect = static_cast<float>(windowSize.x) /
	static_cast<float>(windowSize.y);
    const glm::mat4 perspective = glm::perspective(45.0f, aspect, 0.1f, 1.0f);
    auto cameraSpace = perspective * view;
    GLint cameraSpaceLoc = glGetUniformLocation(lightingProg, "cameraSpace");
    lightSpaceLoc = glGetUniformLocation(lightingProg, "lightSpace");
    glUniformMatrix4fv(cameraSpaceLoc, 1, GL_FALSE, glm::value_ptr(cameraSpace));
    glUniformMatrix4fv(lightSpaceLoc, 1, GL_FALSE, glm::value_ptr(lightSpace));

    glUseProgram(terrainProg);
    cameraSpaceLoc = glGetUniformLocation(terrainProg, "cameraSpace");
    glUniformMatrix4fv(cameraSpaceLoc, 1, GL_FALSE, glm::value_ptr(cameraSpace));

    glUseProgram(genericTxtrdProg);
    cameraSpaceLoc = glGetUniformLocation(genericTxtrdProg, "cameraSpace");
    glUniformMatrix4fv(cameraSpaceLoc, 1, GL_FALSE, glm::value_ptr(cameraSpace));
}

void World::UpdateOrthoProjUniforms() {
    auto & assets = GetGame().GetAssets();
    const GLuint lensFlareProg = assets.GetShaderProgram(ShaderProgramId::LensFlare);
    glUseProgram(lensFlareProg);
    const auto windowSize = GetGame().GetWindowSize();
    const glm::mat4 ortho = glm::ortho(0.f, static_cast<float>(windowSize.x),
				       0.f, static_cast<float>(windowSize.y));
    const GLint projLoc = glGetUniformLocation(lensFlareProg, "proj");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(ortho));
}

void World::Display() {
    auto & game = GetGame();
    std::lock_guard<std::mutex> lk(m_updateMtx);
    UpdatePerspProjUniforms();
    game.GetTerrain().SwapChunks();
    game.DrawShadowMap();
    const auto & windowSize = game.GetWindowSize();
    glViewport(0, 0, windowSize.x, windowSize.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    DrawTerrain();
    DrawSky();
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
    DrawOverlays();
}

void World::DrawOverlays() {
    UpdateOrthoProjUniforms();
    glDisable(GL_DEPTH_TEST);
    GetGame().GetSky().DoLensFlare();
    glEnable(GL_DEPTH_TEST);
}


void Menu::Update(const long long dt) {
    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
	GetGame().PopScene();
    }
}

void Menu::Display() {
    // ...
}
