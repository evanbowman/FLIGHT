#include "../Game.hpp"
#include "../Scene.hpp"

namespace FLIGHT {
std::mutex g_updateMtx;

World::World() {}

void DisplayShadowOverlay(const float amount) {
    glDisable(GL_DEPTH_TEST);
    auto genericProg =
        GetGame().GetAssetMgr().GetProgram<ShaderProgramId::Generic>();
    genericProg->Use();
    const auto windowSize = GetGame().GetWindowSize();
    const glm::mat4 ortho = glm::ortho(0.f, static_cast<float>(windowSize.x),
                                       0.f, static_cast<float>(windowSize.y));
    genericProg->SetUniformMat4("cameraSpace", ortho);
    glm::mat4 model = glm::translate(glm::mat4(1), {0, windowSize.y, 0.f});
    model = glm::scale(model, {windowSize.x, windowSize.y, 0.f});
    Primitives::Quad quad;
    genericProg->SetUniformVec4("color", {0, 0, 0, amount});
    genericProg->SetUniformMat4("model", model);
    quad.Display(*genericProg, AlphaBlend);
    glEnable(GL_DEPTH_TEST);
}

void World::UpdateLogic(const Time dt) {
    auto & camera = GetGame().GetCamera();
    GetGame().GetCollisionMgr().Update();
    {
        std::lock_guard<std::mutex> lk(g_updateMtx);
        GetGame().GetPlayer().Update(dt);
        GetGame().UpdateEntities(dt);
        camera.Update(dt);
    }
    m_reticle.Update(GetGame().GetPlayer());
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

void World::DrawTerrain() {
    auto terrainProg =
        GetGame().GetAssetMgr().GetProgram<ShaderProgramId::Terrain>();
    terrainProg->Use();
    const auto view = GetGame().GetCamera().GetWorldView();
    auto invView = glm::inverse(view);
    glm::vec3 eyePos = invView * glm::vec4(0, 0, 0, 1);
    terrainProg->SetUniformVec3("eyePos", eyePos);
    GetGame().GetTerrainMgr().Display(*terrainProg);
    AssertGLStatus("terrain rendering");
}

void World::DrawSky() { GetGame().GetSkyMgr().Display(); }

static const glm::mat4 LIGHT_PROJ_MAT =
    glm::ortho(-4.f, 4.f, -4.f, 4.f, -5.f, 12.f);

void World::UpdatePerspProjUniforms() {
    auto & assets = GetGame().GetAssetMgr();
    auto shadowProgram = assets.GetProgram<ShaderProgramId::Shadow>();
    auto lightingProg = assets.GetProgram<ShaderProgramId::Base>();
    auto terrainProg = assets.GetProgram<ShaderProgramId::Terrain>();
    auto genericTxtrdProg =
        assets.GetProgram<ShaderProgramId::GenericTextured>();
    auto skyProg = assets.GetProgram<ShaderProgramId::SkyGradient>();
    auto solidColProg = assets.GetProgram<ShaderProgramId::SolidColor3D>();

    shadowProgram->Use();
    auto & camera = GetGame().GetCamera();
    auto view = camera.GetLightView();
    auto lightSpace = LIGHT_PROJ_MAT * view;
    shadowProgram->SetUniformMat4("lightSpace", lightSpace);

    lightingProg->Use();
    view = camera.GetWorldView();
    const auto & windowSize = GetGame().GetWindowSize();
    const float aspect =
        static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);
    const glm::mat4 perspective = glm::perspective(45.0f, aspect, 0.1f, 1.0f);
    auto cameraSpace = perspective * view;
    lightingProg->SetUniformMat4("lightSpace", lightSpace);
    lightingProg->SetUniformMat4("cameraSpace", cameraSpace);

    terrainProg->Use();
    terrainProg->SetUniformMat4("cameraSpace", cameraSpace);

    genericTxtrdProg->Use();
    genericTxtrdProg->SetUniformMat4("cameraSpace", cameraSpace);

    skyProg->Use();
    skyProg->SetUniformMat4("cameraSpace", cameraSpace);

    solidColProg->Use();
    solidColProg->SetUniformMat4("cameraSpace", cameraSpace);
}

void World::UpdateOrthoProjUniforms() {
    auto & assets = GetGame().GetAssetMgr();
    auto lensFlareProg = assets.GetProgram<ShaderProgramId::LensFlare>();
    lensFlareProg->Use();
    const auto windowSize = GetGame().GetWindowSize();
    const glm::mat4 ortho = glm::ortho(0.f, static_cast<float>(windowSize.x),
                                       0.f, static_cast<float>(windowSize.y));
    lensFlareProg->SetUniformMat4("proj", ortho);

    auto reticleProg = assets.GetProgram<ShaderProgramId::Reticle>();
    reticleProg->Use();
    reticleProg->SetUniformMat4("proj", ortho);

    auto reticleShadowProg =
        assets.GetProgram<ShaderProgramId::ReticleShadow>();
    reticleShadowProg->Use();
    reticleShadowProg->SetUniformMat4("proj", ortho);

    auto txtrdQuadProg = assets.GetProgram<ShaderProgramId::GenericTextured>();
    txtrdQuadProg->Use();
    txtrdQuadProg->SetUniformMat4("cameraSpace", ortho);
}

bool World::Display() {
    auto & game = GetGame();
    std::lock_guard<std::mutex> lk(g_updateMtx);
    UpdatePerspProjUniforms();
    game.GetTerrainMgr().SwapChunks();
    game.DrawShadowMap();
    const auto & windowSize = game.GetWindowSize();
    glViewport(0, 0, windowSize.x, windowSize.y);
    glClearColor(0.3f, 0.72f, 1.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    DrawTerrain();
    DrawSky();
    auto lightingProg = game.GetAssetMgr().GetProgram<ShaderProgramId::Base>();
    lightingProg->Use();
    const auto view = game.GetCamera().GetWorldView();
    auto invView = glm::inverse(view);
    glm::vec3 eyePos = invView * glm::vec4(0, 0, 0, 1);
    lightingProg->SetUniformVec3("eyePos", eyePos);
    lightingProg->SetUniformInt("shadowMap", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, game.GetShadowMapTxtr());
    game.GetPlayer().GetPlane()->Display(*lightingProg);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
        auto solidCol3DProg =
            GetGame().GetAssetMgr().GetProgram<ShaderProgramId::SolidColor3D>();
        solidCol3DProg->Use();
        GetGame().GetCollisionMgr().DisplayAABBs(*solidCol3DProg);
    }
    DrawOverlays();
    return true;
}

void World::DrawOverlays() {
    UpdateOrthoProjUniforms();
    glDisable(GL_DEPTH_TEST);
    GetGame().GetSkyMgr().DoLensFlare();
    m_reticle.Display();
    glEnable(GL_DEPTH_TEST);
}
}
