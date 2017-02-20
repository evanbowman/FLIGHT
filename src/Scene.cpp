#include "Scene.hpp"
#include "Game.hpp"

static std::mutex g_updateMtx;

void TitleScreen::UpdateLogic(const long long dt) {
    // ...
}

void TitleScreen::UpdateState(SceneStack & state) {
    state.push(std::make_unique<WorldLoader>());
}

void TitleScreen::Display() {
    // ...
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

void WorldLoader::UpdateLogic(const long long dt) {
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
	state.push(std::make_unique<World>());
    }
}

void WorldLoader::Display() {
    GetGame().GetTerrain().SwapChunks();
}

World::World() {
    
}

bool createdTransIn = false;

void World::UpdateLogic(const long long dt) {
    auto & camera = GetGame().GetCamera();
    {
	std::lock_guard<std::mutex> lk(g_updateMtx);
	GetGame().GetPlayer().Update(dt);
	camera.Update(dt);
    }
    const auto view = camera.GetWorldView();
    auto invView = glm::inverse(view);
    glm::vec3 eyePos = invView * glm::vec4(0, 0, 0, 1);
    GetGame().GetTerrain().UpdateChunkLOD(eyePos, camera.GetViewDir());
    GetGame().GetSky().Update(eyePos, camera.GetViewDir());
}

void World::UpdateState(SceneStack & state) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
	state.push(std::make_unique<MenuTransitionIn>());
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
    std::lock_guard<std::mutex> lk(g_updateMtx);
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

void Menu::UpdateLogic(const long long dt) {
    // ...
}

void Menu::UpdateState(SceneStack & state) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        state.pop();
	state.push(std::make_unique<MenuTransitionOut>());
    }
}

void Menu::Display() {
    // ...
}

static void DisplayShadowOverlay(const float amount) {
    glDisable(GL_DEPTH_TEST);
    const GLuint genericProg = GetGame().GetAssets().GetShaderProgram(ShaderProgramId::Generic);
    glUseProgram(genericProg);
    const auto windowSize = GetGame().GetWindowSize();
    const glm::mat4 ortho = glm::ortho(0.f, static_cast<float>(windowSize.x),
				       0.f, static_cast<float>(windowSize.y));
    const GLint projLoc = glGetUniformLocation(genericProg, "cameraSpace");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(ortho));
    glm::mat4 model = glm::translate(glm::mat4(1), {0, windowSize.y, 0.f});
    model = glm::scale(model, {windowSize.x, windowSize.y, 0.f});
    Primitives::Quad quad;
    const GLint colorLoc = glGetUniformLocation(genericProg, "color");
    glUniform4f(colorLoc, 0, 0, 0, amount);
    const GLint modelLoc = glGetUniformLocation(genericProg, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    quad.Display(genericProg, {BlendMode::Mode::Alpha, BlendMode::Mode::OneMinusAlpha});
    glEnable(GL_DEPTH_TEST);
}

MenuTransitionIn::MenuTransitionIn() : m_transitionTimer(0) {}

void MenuTransitionIn::UpdateLogic(const long long dt) {
    World::UpdateLogic(dt);
    m_transitionTimer += dt;
}

void MenuTransitionIn::UpdateState(SceneStack & state) {
    if (m_transitionTimer > TRANSITION_TIME) {
	state.pop();
	state.push(std::make_unique<Menu>());
    }
}

void MenuTransitionIn::Display() {
    World::Display();
    const float overlayDarkness = glm::smoothstep(0.f, (float)TRANSITION_TIME, (float)m_transitionTimer) / 2.f;
    DisplayShadowOverlay(overlayDarkness);
}

MenuTransitionOut::MenuTransitionOut() : m_transitionTimer(0) {}

void MenuTransitionOut::UpdateLogic(const long long dt) {
    World::UpdateLogic(dt);
    m_transitionTimer += dt;
}

void MenuTransitionOut::UpdateState(SceneStack & state) {
    if (m_transitionTimer > TRANSITION_TIME) {
	state.pop();
    }
}

void MenuTransitionOut::Display() {
    World::Display();
    const float overlayDarkness = 0.5f - glm::smoothstep(0.f, (float)TRANSITION_TIME, (float)m_transitionTimer) / 2.f;
    DisplayShadowOverlay(overlayDarkness);
}
