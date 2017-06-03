#include <FLIGHT/Core/AssetManager.hpp>
#include <FLIGHT/Core/Game.hpp>
#include <FLIGHT/Entity/Plane.hpp>
#include <FLIGHT/Graphics/OpenGLDisplayImpl.hpp>
#include <FLIGHT/Entity/Coin.hpp>

namespace FLIGHT {
void DisplayShadowOverlay(const float amount) {
    auto & game = Singleton<Game>::Instance();
    glDisable(GL_DEPTH_TEST);
    auto & genericProg =
        game.GetAssetMgr().GetProgram<ShaderProgramId::Generic>();
    genericProg.Use();
    const auto windowSize = game.GetSubwindowSize();
    const glm::mat4 ortho = glm::ortho(0.f, static_cast<float>(windowSize.x),
                                       0.f, static_cast<float>(windowSize.y));
    genericProg.SetUniformMat4("cameraSpace", ortho);
    glm::mat4 model = glm::translate(glm::mat4(1), {0, windowSize.y, 0.f});
    model = glm::scale(model, {windowSize.x, windowSize.y, 0.f});
    PRIMITIVES::Quad quad;
    genericProg.SetUniformVec4("color", {0, 0, 0, amount});
    genericProg.SetUniformMat4("model", model);
    quad.Display(genericProg, AlphaBlend);
    glEnable(GL_DEPTH_TEST);
}

static const glm::mat4 LIGHT_PROJ_MAT =
    glm::ortho(-4.f, 4.f, -4.f, 4.f, -5.f, 12.f);

static void UpdatePerspProjUniforms() {
    auto & game = Singleton<Game>::Instance();
    auto & assets = game.GetAssetMgr();
    auto & shadowProgram = assets.GetProgram<ShaderProgramId::Shadow>();
    auto & lightingProg = assets.GetProgram<ShaderProgramId::Base>();
    auto & terrainProg = assets.GetProgram<ShaderProgramId::Terrain>();
    auto & genericTxtrdProg =
        assets.GetProgram<ShaderProgramId::GenericTextured>();
    auto & skyProg = assets.GetProgram<ShaderProgramId::SkyGradient>();
    auto & solidColProg = assets.GetProgram<ShaderProgramId::SolidColor3D>();

    shadowProgram.Use();
    auto & camera = game.GetCamera();
    auto view = camera.GetLightView();
    auto lightSpace = LIGHT_PROJ_MAT * view;
    shadowProgram.SetUniformMat4("lightSpace", lightSpace);

    lightingProg.Use();
    view = camera.GetWorldView();
    const auto & windowSize = game.GetSubwindowSize();
    const float aspect =
        static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);
    const glm::mat4 perspective = glm::perspective(45.0f, aspect, 0.1f, 1.0f);
    auto cameraSpace = perspective * view;
    lightingProg.SetUniformMat4("lightSpace", lightSpace);
    lightingProg.SetUniformMat4("cameraSpace", cameraSpace);

    terrainProg.Use();
    terrainProg.SetUniformMat4("cameraSpace", cameraSpace);

    genericTxtrdProg.Use();
    genericTxtrdProg.SetUniformMat4("cameraSpace", cameraSpace);

    skyProg.Use();
    skyProg.SetUniformMat4("cameraSpace", cameraSpace);

    solidColProg.Use();
    solidColProg.SetUniformMat4("cameraSpace", cameraSpace);
}

static void DrawTerrain(OpenGLDisplayImpl & gl) {
    auto & game = Singleton<Game>::Instance();
    auto & terrainProg =
        game.GetAssetMgr().GetProgram<ShaderProgramId::Terrain>();
    terrainProg.Use();
    const auto view = game.GetCamera().GetWorldView();
    auto invView = glm::inverse(view);
    glm::vec3 eyePos = invView * glm::vec4(0, 0, 0, 1);
    terrainProg.SetUniformVec3("eyePos", eyePos);
    Singleton<Game>::Instance().GetTerrainMgr().Display(gl);
    AssertGLStatus("terrain rendering");
}

static void UpdateOrthoProjUniforms() {
    auto & game = Singleton<Game>::Instance();
    auto & assets = game.GetAssetMgr();
    auto & lensFlareProg = assets.GetProgram<ShaderProgramId::LensFlare>();
    lensFlareProg.Use();
    const auto windowSize = game.GetSubwindowSize();
    const glm::mat4 ortho = glm::ortho(0.f, static_cast<float>(windowSize.x),
                                       0.f, static_cast<float>(windowSize.y));
    lensFlareProg.SetUniformMat4("proj", ortho);

    auto & reticleProg = assets.GetProgram<ShaderProgramId::Reticle>();
    reticleProg.Use();
    reticleProg.SetUniformMat4("proj", ortho);

    auto & powerupProgBG = assets.GetProgram<ShaderProgramId::PowerupBG>();
    powerupProgBG.Use();
    powerupProgBG.SetUniformMat4("proj", ortho);

    auto & powerupProgFG = assets.GetProgram<ShaderProgramId::PowerupFG>();
    powerupProgFG.Use();
    powerupProgFG.SetUniformMat4("proj", ortho);

    auto & reticleShadowProg =
        assets.GetProgram<ShaderProgramId::ReticleShadow>();
    reticleShadowProg.Use();
    reticleShadowProg.SetUniformMat4("proj", ortho);

    auto & txtrdQuadProg =
        assets.GetProgram<ShaderProgramId::GenericTextured>();
    txtrdQuadProg.Use();
    txtrdQuadProg.SetUniformMat4("cameraSpace", ortho);

    auto & fontShader = assets.GetProgram<ShaderProgramId::Font>();
    fontShader.Use();
    fontShader.SetUniformMat4("proj", ortho);
}

extern std::array<SkyManager::Flare, 11> g_lensFlares;

static void DoLensFlare() {
    auto & game = Singleton<Game>::Instance();
    if (game.GetSkyMgr().SunVisible()) {
        auto & lensFlareProg =
            game.GetAssetMgr().GetProgram<ShaderProgramId::LensFlare>();
        lensFlareProg.Use();
        for (const auto & flare : g_lensFlares) {
            lensFlareProg.SetUniformFloat("intensity", 0.3f * flare.intensity);
            PRIMITIVES::Hexagon hex;
            glm::mat4 model = glm::translate(glm::mat4(1), flare.position);
            model = glm::scale(model, {flare.scale, flare.scale, flare.scale});
            lensFlareProg.SetUniformMat4("model", model);
            hex.Display(lensFlareProg, AdditiveBlend);
        }
    }
}

inline static void DrawPowerups() {
    auto & game = Singleton<Game>::Instance();
    auto & assets = game.GetAssetMgr();
    const auto & windowSize = game.GetSubwindowSize();
    auto mark = windowSize;
    static const float bubbleRadius = CalcPowerupIconSize(windowSize);
    mark.x -= bubbleRadius * 2.f;
    auto & powerupProgBG = assets.GetProgram<ShaderProgramId::PowerupBG>();
    auto & powerupProgFG = assets.GetProgram<ShaderProgramId::PowerupFG>();
    PRIMITIVES::TexturedQuad quad;
    for (auto powerup : game.GetPlayer1().GetPowerups()) {
        if (powerup == Powerup::None) {
            continue;
        }
        powerupProgBG.Use();
        mark.y -= bubbleRadius * 2.5f;
        const float p1Score = game.GetPlayer1().GetDelayedScore();
        const Score cost = GetCost(powerup);
        glm::mat4 model = glm::translate(glm::mat4(1), {mark.x, mark.y, 0.f});
        model = glm::scale(model, {bubbleRadius, bubbleRadius, 0.f});
        powerupProgBG.SetUniformMat4("model", model);
        quad.Display(powerupProgBG, AlphaBlend);
        powerupProgFG.Use();
        const float fillRatio = std::min(1.f, p1Score / cost);
        powerupProgFG.SetUniformFloat("fill", fillRatio);
        powerupProgFG.SetUniformMat4("model", model);
        glActiveTexture(GL_TEXTURE0);
        powerupProgFG.SetUniformInt("tex", 0);
        std::shared_ptr<Texture> icon;
        switch (powerup) {
        case Powerup::Dash:
            icon = assets.GetPowerupIcon<Powerup::Dash>();
            break;

        case Powerup::Pulse:
            icon = assets.GetPowerupIcon<Powerup::Pulse>();
            break;

        case Powerup::Heal:
            icon = assets.GetPowerupIcon<Powerup::Heal>();
            break;
        }
        glBindTexture(GL_TEXTURE_2D, icon->GetId());
        quad.Display(powerupProgFG, AlphaBlend);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

static void DrawOverlays() {
    UpdateOrthoProjUniforms();
    glDisable(GL_DEPTH_TEST);
    Singleton<Game>::Instance().GetCamera().DisplayOverlay();
    DoLensFlare();
    DrawPowerups();
    glEnable(GL_DEPTH_TEST);
}

constexpr static size_t GetChunkIndexCount(const size_t scale) {
    return ((((CHUNK_SIZE + TerrainChunk::GetMargin()) / scale) - 1) *
            (((CHUNK_SIZE + TerrainChunk::GetMargin()) / scale) - 1)) *
           6;
}

void OpenGLDisplayImpl::InitChunkIndexBufs() {
    constexpr const size_t chunkSize = CHUNK_SIZE + TerrainChunk::GetMargin();
    MeshBuilder meshBuilderHQ(chunkSize, chunkSize);
    MeshBuilder meshBuilderMQ(chunkSize, chunkSize);
    MeshBuilder meshBuilderLQ(chunkSize, chunkSize);
    MeshBuilder meshBuilderDQ(chunkSize, chunkSize);
    int vertIndex = 0;
    for (size_t y = 0; y < chunkSize; ++y) {
        for (size_t x = 0; x < chunkSize; ++x) {
            if (x < chunkSize - 1 and y < chunkSize - 1) {
                meshBuilderHQ.AddTriangle(vertIndex, vertIndex + chunkSize + 1,
                                          vertIndex + chunkSize);
                meshBuilderHQ.AddTriangle(vertIndex + chunkSize + 1, vertIndex,
                                          vertIndex + 1);
                if (x % 2 == 0 and y % 2 == 0 and x < chunkSize - 2 and
                    y < chunkSize - 2) {
                    meshBuilderMQ.AddTriangle(vertIndex,
                                              vertIndex + chunkSize * 2 + 2,
                                              vertIndex + chunkSize * 2);
                    meshBuilderMQ.AddTriangle(vertIndex + chunkSize * 2 + 2,
                                              vertIndex, vertIndex + 2);
                }
                if (x % 3 == 0 and y % 3 == 0 and x < chunkSize - 3 and
                    y < chunkSize - 3) {
                    meshBuilderLQ.AddTriangle(vertIndex,
                                              vertIndex + chunkSize * 3 + 3,
                                              vertIndex + chunkSize * 3);
                    meshBuilderLQ.AddTriangle(vertIndex + chunkSize * 3 + 3,
                                              vertIndex, vertIndex + 3);
                }
                if (x % 4 == 0 and y % 4 == 0 and x < chunkSize - 4 and
                    y < chunkSize - 4) {
                    meshBuilderDQ.AddTriangle(vertIndex,
                                              vertIndex + chunkSize * 4 + 4,
                                              vertIndex + chunkSize * 4);
                    meshBuilderDQ.AddTriangle(vertIndex + chunkSize * 4 + 4,
                                              vertIndex, vertIndex + 4);
                }
            }
            ++vertIndex;
        }
    }
    auto meshHQ = meshBuilderHQ.GetMesh();
    auto meshMQ = meshBuilderMQ.GetMesh();
    auto meshLQ = meshBuilderLQ.GetMesh();
    auto meshDQ = meshBuilderDQ.GetMesh();
    glGenBuffers(1, &m_chunkInfo.chunkIndicesHQ);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_chunkInfo.chunkIndicesHQ);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 GetChunkIndexCount(1) * sizeof(GLushort),
                 meshHQ.triangles.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &m_chunkInfo.chunkIndicesMQ);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_chunkInfo.chunkIndicesMQ);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 GetChunkIndexCount(2) * sizeof(GLushort),
                 meshMQ.triangles.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &m_chunkInfo.chunkIndicesLQ);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_chunkInfo.chunkIndicesLQ);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 GetChunkIndexCount(3) * sizeof(GLushort),
                 meshLQ.triangles.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &m_chunkInfo.chunkIndicesDQ);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_chunkInfo.chunkIndicesDQ);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 GetChunkIndexCount(4) * sizeof(GLushort),
                 meshDQ.triangles.data(), GL_STATIC_DRAW);
}

void OpenGLDisplayImpl::DrawShadowMap() {
    auto & game = Singleton<Game>::Instance();
    auto & shadowProgram =
        game.GetAssetMgr().GetProgram<ShaderProgramId::Shadow>();
    shadowProgram.Use();
    const auto & windowSize = game.GetSubwindowSize();
    glViewport(0, 0, windowSize.x / 2, windowSize.y / 2);
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    if (auto plane = game.GetPlayer1().GetPlane()) {
        plane->CastShadow(shadowProgram);
    }
    AssertGLStatus("shadow loop");
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) not_eq
        GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Incomplete framebuffer");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLDisplayImpl::SetupShadowMap() {
    auto & game = Singleton<Game>::Instance();
    glGenFramebuffers(1, &m_shadowMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);
    glGenTextures(1, &m_shadowMapTxtr);
    glBindTexture(GL_TEXTURE_2D, m_shadowMapTxtr);
    const auto & windowSize = game.GetSubwindowSize();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowSize.x / 2,
                 windowSize.y / 2, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           m_shadowMapTxtr, 0);
    glDrawBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) not_eq
        GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Unable to set up frame buffer");
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void LoadExtensions() {
#ifndef __APPLE__
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err not_eq GLEW_OK) {
        throw std::runtime_error(
            std::string((const char *)glewGetErrorString(err)));
    }
    // Note: on some platforms using glewExperimental results in a
    // GL_INVALID_ENUM (code 0x500)
    err = glGetError();
    if (err not_eq GL_INVALID_ENUM and err not_eq GL_NO_ERROR) {
        std::stringstream ss;
        ss << "GL error in glew initialization, code: " << std::hex << err
           << std::dec;
        throw std::runtime_error(ss.str());
    }
#endif
}

OpenGLDisplayImpl::OpenGLDisplayImpl() {
    LoadExtensions();
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    InitChunkIndexBufs();
    PRIMITIVES::Init();
    Text::Enable();
    SetupShadowMap();
}

OpenGLDisplayImpl::~OpenGLDisplayImpl() { glDeleteVertexArrays(1, &m_vao); }

void OpenGLDisplayImpl::Dispatch(Plane & plane) {
    auto & shader = Singleton<Game>::Instance()
                        .GetAssetMgr()
                        .GetProgram<ShaderProgramId::Base>();
    shader.Use();
    glm::mat4 modelMatrix;
    modelMatrix = glm::translate(modelMatrix, plane.GetPosition());
    const auto & rot = plane.GetRotation();
    modelMatrix = glm::rotate(modelMatrix, rot.y, {0, 1, 0});
    modelMatrix = glm::rotate(modelMatrix, rot.z, {0, 0, 1});
    modelMatrix = glm::rotate(modelMatrix, rot.x, {1, 0, 0});
    for (auto & part : plane.GetParts()) {
        part.Display(modelMatrix, shader);
    }
}

void OpenGLDisplayImpl::Dispatch(Coin & coin) {
    auto & game = Singleton<Game>::Instance();
    auto & shader =
        game.GetAssetMgr().GetProgram<ShaderProgramId::SolidColor3D>();
    shader.Use();
    if (auto modelSp = game.GetAssetMgr().GetModel("Box.obj")) {
        auto binding = modelSp->Bind(shader);
        glm::mat4 model;
        model = glm::translate(model, coin.GetPosition());
        shader.SetUniformMat4("model", model);
        shader.SetUniformVec4("color", {0.949f, 0.765f, 0.027f, 1.f});
        glDrawArrays(GL_TRIANGLES, 0, binding.numVertices);
    }
}

void OpenGLDisplayImpl::Dispatch(TerrainChunk & chunk) {
    auto & shader = Singleton<Game>::Instance()
                        .GetAssetMgr()
                        .GetProgram<ShaderProgramId::Terrain>();
    shader.Use();
    glm::mat4 model;
    model = glm::translate(model, chunk.GetPosition());
    glm::mat4 invTransModel = glm::transpose(glm::inverse(model));
    shader.SetUniformMat4("invTransModel", invTransModel);
    shader.SetUniformMat4("model", model);
    chunk.GetMeshData().Bind();
    shader.SetVertexAttribPtr("position", 3, GL_FLOAT, sizeof(VertexPN));
    shader.SetVertexAttribPtr("normal", 3, GL_FLOAT, sizeof(VertexPN),
                              sizeof(glm::vec3));
    switch (chunk.GetDrawQuality()) {
    case TerrainChunk::DrawQuality::High:
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_chunkInfo.chunkIndicesHQ);
        glDrawElements(GL_TRIANGLES, GetChunkIndexCount(1), GL_UNSIGNED_SHORT,
                       0);
        break;

    case TerrainChunk::DrawQuality::Medium:
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_chunkInfo.chunkIndicesMQ);
        glDrawElements(GL_TRIANGLES, GetChunkIndexCount(2), GL_UNSIGNED_SHORT,
                       0);
        break;

    case TerrainChunk::DrawQuality::Low:
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_chunkInfo.chunkIndicesLQ);
        glDrawElements(GL_TRIANGLES, GetChunkIndexCount(3), GL_UNSIGNED_SHORT,
                       0);
        break;

    case TerrainChunk::DrawQuality::Despicable:
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_chunkInfo.chunkIndicesDQ);
        glDrawElements(GL_TRIANGLES, GetChunkIndexCount(4), GL_UNSIGNED_SHORT,
                       0);
        break;

    case TerrainChunk::DrawQuality::None:
        break;
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLDisplayImpl::Dispatch(Bullet & bullet) {
    // TODO...
}

void OpenGLDisplayImpl::Dispatch(Thruster & thruster) {
    // TODO...
}

void OpenGLDisplayImpl::Dispatch(TitleScreen & titleScreen) {
    // TODO...
}

void OpenGLDisplayImpl::Dispatch(CreditsScreen & creditsScreen) {
    auto & game = Singleton<Game>::Instance();
    const auto & windowSize = game.GetSubwindowSize();
    const glm::mat4 ortho = glm::ortho(0.f, static_cast<float>(windowSize.x),
                                       0.f, static_cast<float>(windowSize.y));
    auto & fontShader = game.GetAssetMgr().GetProgram<ShaderProgramId::Font>();
    fontShader.Use();
    fontShader.SetUniformMat4("proj", ortho);
    glViewport(0, 0, windowSize.x, windowSize.y);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    creditsScreen.GetText().Display();
    glEnable(GL_DEPTH_TEST);
    AssertGLStatus("credits screen");
}

void OpenGLDisplayImpl::Dispatch(WorldLoader & worldLoader) {
    // ...
}

void OpenGLDisplayImpl::Dispatch(World & world) {
    auto & game = Singleton<Game>::Instance();
    UpdatePerspProjUniforms();
    game.GetTerrainMgr().SwapChunks();
    DrawShadowMap();
    const auto & windowSize = game.GetSubwindowSize();
    glViewport(0, 0, windowSize.x, windowSize.y);
    glClearColor(0.3f, 0.72f, 1.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    DrawTerrain(*this);
    game.GetSkyMgr().Display(*this);
    auto & lightingProg =
        game.GetAssetMgr().GetProgram<ShaderProgramId::Base>();
    lightingProg.Use();
    const auto view = game.GetCamera().GetWorldView();
    auto invView = glm::inverse(view);
    glm::vec3 eyePos = invView * glm::vec4(0, 0, 0, 1);
    if (auto playerPlane = game.GetPlayer1().GetPlane()) {
        lightingProg.SetUniformVec3("eyePos", eyePos);
        lightingProg.SetUniformInt("shadowMap", 1);
        lightingProg.SetUniformFloat(
            "overrideColorAmount",
            game.GetPlayer1().GetPlane()->GetMixAmount());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_shadowMapTxtr);
        playerPlane->Display(*this);
    }
    DrawOverlays();
}

void OpenGLDisplayImpl::Dispatch(WorldTransitionIn & wtin) {
    const float overlayDarkness =
        1.f - glm::smoothstep(0.f, static_cast<float>(wtin.TRANSITION_TIME),
                              static_cast<float>(wtin.GetTimer()));
    DisplayShadowOverlay(overlayDarkness);
}

void OpenGLDisplayImpl::Dispatch(SkyManager & sky) {
    auto & game = Singleton<Game>::Instance();
    auto & skyProg =
        game.GetAssetMgr().GetProgram<ShaderProgramId::SkyGradient>();
    skyProg.Use();
    const auto & skydomeLocus = game.GetSkyMgr().GetSkydomeCenter();
    const auto & skydomeRot = game.GetSkyMgr().GetSkydomeRot();
    glm::mat4 skyBgModel =
        glm::translate(glm::mat4(1), {skydomeLocus.x, 0, skydomeLocus.z});
    skyBgModel = glm::scale(skyBgModel, {400.f, 400.f, 400.f});
    skyBgModel = glm::rotate(skyBgModel, skydomeRot.y, {0, 1, 0});
    skyProg.SetUniformMat4("model", skyBgModel);
    auto binding = game.GetAssetMgr().GetModel("SkyDome.obj")->Bind(skyProg);
    glDrawArrays(GL_TRIANGLES, 0, binding.numVertices);
    if (game.GetSkyMgr().SunVisible()) {
        auto & textrdQuadProg =
            game.GetAssetMgr().GetProgram<ShaderProgramId::GenericTextured>();
        textrdQuadProg.Use();
        glActiveTexture(GL_TEXTURE1);
        textrdQuadProg.SetUniformInt("tex", 1);
        glBindTexture(GL_TEXTURE_2D,
                      game.GetAssetMgr().GetTexture("Sun.png")->GetId());
        glm::mat4 model;
        model = glm::translate(model, game.GetSkyMgr().GetSunPos());
        model = glm::scale(model, {15.f, 15.f, 15.f});
        model = glm::rotate(model, skydomeRot.y, {0, 1, 0});
        model = glm::rotate(model, -skydomeRot.x, {1, 0, 0});
        textrdQuadProg.SetUniformMat4("model", model);
        PRIMITIVES::TexturedQuad quad;
        quad.Display(textrdQuadProg, AdditiveBlend);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    AssertGLStatus("rendering sky");
}
}
