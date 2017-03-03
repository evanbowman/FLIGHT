#include "Sky.hpp"
#include "Game.hpp"

namespace FLIGHT {    
    static std::array<SkyManager::Flare, 11> g_lensFlares{
	{{18.f, 0.9f, {}, 1.f},
	 {46.f, 0.45f, {}, 1.f},
	 {100.f, 0.6f, {}, 1.f},
	 {200.f, 0.4f, {}, 1.f},
	 {300.f, 1.2f, {}, 1.f},
	 {360.f, 0.15f, {}, 1.f},
	 {500.f, 2.5f, {}, 1.f},
	 {525.f, 0.4f, {}, 1.f},
	 {1350.f, 4.5f, {}, 1.f},
	 {1700.f, 3.6f, {}, 1.f},
	 {1850.f, 4.8f, {}, 1.f}}};

    void SkyManager::Update(const glm::vec3 & cameraPos,
			    const glm::vec3 & viewDir) {
	m_sunPos = cameraPos - glm::vec3{0, 0, 330};
	m_sunPos.y = 180;
	m_skydomeLocus = cameraPos;
	m_sunVisible = IntersectsFrustum(m_sunPos, cameraPos, viewDir);
	m_rot.y = std::atan2f(viewDir.x, viewDir.z);
	m_rot.x = std::atan2f(viewDir.y, viewDir.z);
	if (m_sunVisible) {
	    auto windowSize = GetGame().GetWindowSize();
	    glm::mat4 view = GetGame().GetCamera().GetWorldView();
	    glm::mat4 proj = glm::perspective(
					      45.0f, windowSize.x / (float)windowSize.y, 0.1f, 1.0f);
	    glm::vec4 viewPort = {0, 0, windowSize.x, windowSize.y};
	    auto sunScreenCoord = glm::project(m_sunPos, view, proj, viewPort);
	    // The frustum check can be inexact for far away things (eg the sun),
	    // and
	    // it's really important here
	    // to only draw the lens flare when the sun is visible in the view!
	    static const float sunRad = 48.f;
	    if (sunScreenCoord.y > windowSize.y + sunRad) {
		m_sunVisible = false;
		return;
	    }
	    glm::vec3 directionVec = {
		glm::vec3{windowSize.x / 2, windowSize.y / 2, 0.f} -
		glm::vec3{sunScreenCoord.x, sunScreenCoord.y, 0.f}};
	    const float dist = glm::length(directionVec);
	    directionVec = glm::normalize(directionVec);
	    const float distRatioToMax = dist / 900.f;
	    for (auto & flare : g_lensFlares) {
		flare.position =
		    glm::vec3{sunScreenCoord.x, sunScreenCoord.y, 0.f} +
									   glm::vec3(directionVec * flare.displFactor * distRatioToMax);
		flare.intensity = 1.f - distRatioToMax;
	    }
	}
    }

    void SkyManager::Display() {
	auto skyProg =
	    GetGame().GetAssetMgr().GetProgram<ShaderProgramId::SkyGradient>();
	skyProg->Use();
	glm::mat4 skyBgModel =
	    glm::translate(glm::mat4(1), {m_skydomeLocus.x, 0, m_skydomeLocus.z});
	skyBgModel = glm::scale(skyBgModel, {400.f, 400.f, 400.f});
	skyBgModel = glm::rotate(skyBgModel, m_rot.y, {0, 1, 0});
	skyProg->SetUniformMat4("model", skyBgModel);
	auto vertices =
	    GetGame().GetAssetMgr().GetModel<ModelId::SkyDome>()->Bind(*skyProg);
	glDrawArrays(GL_TRIANGLES, 0, vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	if (m_sunVisible) {
	    auto textrdQuadProg =
		GetGame()
                .GetAssetMgr()
                .GetProgram<ShaderProgramId::GenericTextured>();
	    textrdQuadProg->Use();
	    glActiveTexture(GL_TEXTURE1);
	    textrdQuadProg->SetUniformInt("tex", 1);
	    glBindTexture(
			  GL_TEXTURE_2D,
			  GetGame().GetAssetMgr().GetTexture<TextureId::Sun>()->GetId());
	    glm::mat4 model;
	    model = glm::translate(model, m_sunPos);
	    model = glm::scale(model, {15.f, 15.f, 15.f});
	    model = glm::rotate(model, m_rot.y, {0, 1, 0});
	    model = glm::rotate(model, -m_rot.x, {1, 0, 0});
	    textrdQuadProg->SetUniformMat4("model", model);
	    Primitives::TexturedQuad quad;
	    quad.Display(*textrdQuadProg, AdditiveBlend);
	    glBindTexture(GL_TEXTURE_2D, 0);
	}
	AssertGLStatus("rendering sky");
    }

    void SkyManager::DoLensFlare() {
	if (m_sunVisible) {
	    auto lensFlareProg =
		GetGame().GetAssetMgr().GetProgram<ShaderProgramId::LensFlare>();
	    lensFlareProg->Use();
	    for (const auto & flare : g_lensFlares) {
		lensFlareProg->SetUniformFloat("intensity", 0.3f * flare.intensity);
		Primitives::Hexagon hex;
		glm::mat4 model = glm::translate(glm::mat4(1), flare.position);
		model = glm::scale(model, {flare.scale, flare.scale, flare.scale});
		lensFlareProg->SetUniformMat4("model", model);
		hex.Display(*lensFlareProg, AdditiveBlend);
	    }
	}
    }
}
