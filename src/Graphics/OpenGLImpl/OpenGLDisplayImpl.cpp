#include <FLIGHT/Graphics/DisplayDispatcher.hpp>
#include <OpenGL/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <FLIGHT/Core/AssetManager.hpp>
#include <FLIGHT/Entity/Plane.hpp>
#include <FLIGHT/Core/Game.hpp>

namespace FLIGHT {
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
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        creditsScreen.GetText().Display();
        glEnable(GL_DEPTH_TEST);
    }

    void OpenGLDisplayImpl::Dispatch(WorldLoader & worldLoader) {
	// TODO...
    }

    void OpenGLDisplayImpl::Dispatch(World & world) {
	// TODO...
    }

    void OpenGLDisplayImpl::Dispatch(WorldTransitionIn & wtin) {
	// TODO...
    }

    void OpenGLDisplayImpl::Dispatch(WorldTransitionOut & wtout) {
	// TODO...
    }

    void OpenGLDisplayImpl::Dispatch(Menu & menu) {
	// TODO...
    }

    void OpenGLDisplayImpl::Dispatch(MenuTransitionIn & mtin) {
	// TODO...
    }

    void OpenGLDisplayImpl::Dispatch(MenuTransitionOut & mtout) {
	// TODO...
    }

    void OpenGLDisplayImpl::Dispatch(SkyManager & sky) {
	// TODO...
    }
}
