#include <FLIGHT/Graphics/DisplayImpl.hpp>
#include <OpenGL/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <FLIGHT/Core/AssetManager.hpp>
#include <FLIGHT/Entity/Plane.hpp>
#include <FLIGHT/Core/Game.hpp>

namespace FLIGHT {
    void DisplayShadowOverlay(const float amount) {
	glDisable(GL_DEPTH_TEST);
	auto & genericProg =
	    GetGame().GetAssetMgr().GetProgram<ShaderProgramId::Generic>();
	genericProg.Use();
	const auto windowSize = GetGame().GetWindowSize();
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
	auto & assets = GetGame().GetAssetMgr();
	auto & shadowProgram = assets.GetProgram<ShaderProgramId::Shadow>();
	auto & lightingProg = assets.GetProgram<ShaderProgramId::Base>();
	auto & terrainProg = assets.GetProgram<ShaderProgramId::Terrain>();
	auto & genericTxtrdProg =
	    assets.GetProgram<ShaderProgramId::GenericTextured>();
	auto & skyProg = assets.GetProgram<ShaderProgramId::SkyGradient>();
	auto & solidColProg = assets.GetProgram<ShaderProgramId::SolidColor3D>();

	shadowProgram.Use();
	auto & camera = GetGame().GetCamera();
	auto view = camera.GetLightView();
	auto lightSpace = LIGHT_PROJ_MAT * view;
	shadowProgram.SetUniformMat4("lightSpace", lightSpace);

	lightingProg.Use();
	view = camera.GetWorldView();
	const auto & windowSize = GetGame().GetWindowSize();
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
	auto & terrainProg =
	    GetGame().GetAssetMgr().GetProgram<ShaderProgramId::Terrain>();
	terrainProg.Use();
	const auto view = GetGame().GetCamera().GetWorldView();
	auto invView = glm::inverse(view);
	glm::vec3 eyePos = invView * glm::vec4(0, 0, 0, 1);
	terrainProg.SetUniformVec3("eyePos", eyePos);
	GetGame().GetTerrainMgr().Display(gl);
	AssertGLStatus("terrain rendering");
    }

    static void UpdateOrthoProjUniforms() {
	auto & assets = GetGame().GetAssetMgr();
	auto & lensFlareProg = assets.GetProgram<ShaderProgramId::LensFlare>();
	lensFlareProg.Use();
	const auto windowSize = GetGame().GetWindowSize();
	const glm::mat4 ortho = glm::ortho(0.f, static_cast<float>(windowSize.x),
					   0.f, static_cast<float>(windowSize.y));
	lensFlareProg.SetUniformMat4("proj", ortho);

	auto & reticleProg = assets.GetProgram<ShaderProgramId::Reticle>();
	reticleProg.Use();
	reticleProg.SetUniformMat4("proj", ortho);

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

    static void DrawOverlays() {
	UpdateOrthoProjUniforms();
	glDisable(GL_DEPTH_TEST);
	GetGame().GetSkyMgr().DoLensFlare();
	GetGame().GetCamera().DisplayOverlay();
	glEnable(GL_DEPTH_TEST);
    }
    
    void OpenGLDisplayImpl::Dispatch(Plane & plane) {
	auto & shader = GetGame().GetAssetMgr().GetProgram<ShaderProgramId::Base>();
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
	auto & shader = GetGame().GetAssetMgr().GetProgram<ShaderProgramId::SolidColor3D>();
        shader.Use();
        if (auto modelSp = GetGame().GetAssetMgr().GetModel("Box.obj")) {
	    auto binding = modelSp->Bind(shader);
	    glm::mat4 model;
	    model = glm::translate(model, coin.GetPosition());
	    shader.SetUniformMat4("model", model);
	    shader.SetUniformVec4("color", {0.949f, 0.765f, 0.027f, 1.f});
	    glDrawArrays(GL_TRIANGLES, 0, binding.numVertices);
	}
    }

    void OpenGLDisplayImpl::Dispatch(TerrainChunk & chunk) {
	auto & shader = GetGame().GetAssetMgr().GetProgram<ShaderProgramId::Terrain>();
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
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk.m_indicesHQ);
	    glDrawElements(GL_TRIANGLES, TerrainChunk::GetIndexCountHQ(),
			   GL_UNSIGNED_SHORT, 0);
	    break;

	case TerrainChunk::DrawQuality::Medium:
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk.m_indicesMQ);
	    glDrawElements(GL_TRIANGLES, TerrainChunk::GetIndexCountMQ(),
			   GL_UNSIGNED_SHORT, 0);
	    break;

	case TerrainChunk::DrawQuality::Low:
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk.m_indicesLQ);
	    glDrawElements(GL_TRIANGLES, TerrainChunk::GetIndexCountLQ(),
			   GL_UNSIGNED_SHORT, 0);
	    break;

	case TerrainChunk::DrawQuality::Despicable:
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk.m_indicesDQ);
	    glDrawElements(GL_TRIANGLES, TerrainChunk::GetIndexCountDQ(),
			   GL_UNSIGNED_SHORT, 0);
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
	const auto & windowSize = GetGame().GetWindowSize();
        const glm::mat4 ortho =
            glm::ortho(0.f, static_cast<float>(windowSize.x), 0.f,
                       static_cast<float>(windowSize.y));
        auto & fontShader =
            GetGame().GetAssetMgr().GetProgram<ShaderProgramId::Font>();
        fontShader.Use();
        fontShader.SetUniformMat4("proj", ortho);
        glViewport(0, 0, windowSize.x, windowSize.y);
	glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        creditsScreen.GetText().Display();
        glEnable(GL_DEPTH_TEST);
    }

    void OpenGLDisplayImpl::Dispatch(WorldLoader & worldLoader) {
	// ...
    }
    
    void OpenGLDisplayImpl::Dispatch(World & world) {
	auto & game = GetGame();
	UpdatePerspProjUniforms();
	game.GetTerrainMgr().SwapChunks();
	game.DrawShadowMap();
	const auto & windowSize = game.GetWindowSize();
	glViewport(0, 0, windowSize.x, windowSize.y);
	glClearColor(0.3f, 0.72f, 1.0f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawTerrain(*this);
	GetGame().GetSkyMgr().Display(); 
	auto & lightingProg =
	    game.GetAssetMgr().GetProgram<ShaderProgramId::Base>();
	lightingProg.Use();
	const auto view = game.GetCamera().GetWorldView();
	auto invView = glm::inverse(view);
	glm::vec3 eyePos = invView * glm::vec4(0, 0, 0, 1);
	if (auto playerPlane = game.GetPlayer().GetPlane()) {
	    lightingProg.SetUniformVec3("eyePos", eyePos);
	    lightingProg.SetUniformInt("shadowMap", 1);
	    lightingProg.SetUniformFloat("overrideColorAmount", game.GetPlayer().GetPlane()->GetMixAmount());
	    glActiveTexture(GL_TEXTURE1);
	    glBindTexture(GL_TEXTURE_2D, game.GetShadowMapTxtr());
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
	// TODO...
    }
}
