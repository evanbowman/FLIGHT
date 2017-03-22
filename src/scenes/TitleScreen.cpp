#include "../Game.hpp"
#include "../Scene.hpp"

namespace FLIGHT {
TitleScreen::TitleScreen() {
    const auto & windowSize = GetGame().GetWindowSize();
    m_titleText.SetFace(GetGame().GetAssetMgr().GetFont<FontSize::Large>());
    m_startText.SetFace(GetGame().GetAssetMgr().GetFont<FontSize::Small>());
    m_titleText.SetString(GetGame().GetConf().localization.strings.appName);
    m_startText.SetString(GetGame().GetConf().localization.strings.startText);
    m_startText.SetColor({1.f, 1.f, 1.f, 0.5f});
    m_titleText.SetPosition({windowSize.x / 2 - m_titleText.GetSize().x / 2.f,
			     windowSize.y - 2.5 * m_titleText.GetSize().y, 0.f});
    m_startText.SetPosition({windowSize.x / 2 - m_startText.GetSize().x / 2.f, 4 * m_startText.GetSize().y, 0.f});
    auto camera = std::make_unique<ElasticCamera>();
    camera->SetPosition({-0.5, 0.2, -2});
    m_plane = GetGame().CreateSolid<Plane>(GetGame().GetPlaneRegistry()["RedTail"]);
    m_plane->SetThrust(0.f);
    m_plane->SetRotation({0.15, 0, 0.433});
    GetGame().GetPlayer().GivePlane(m_plane);
    camera->SetTarget(m_plane);
    GetGame().SetCamera(std::move(camera));
}
    
void TitleScreen::UpdateLogic(const Time dt) {
    m_startTextTimer += dt;
    float alpha = 1.f - (1.f + std::sin(2 * UTIL::PI * 0.000001 * m_startTextTimer)) / 3.f;
    m_startText.SetColor({1.f, 1.f, 1.f, alpha});
    GetGame().GetCamera().Update(dt);
}

    static const glm::mat4 LIGHT_PROJ_MAT =
    glm::ortho(-4.f, 4.f, -4.f, 4.f, -5.f, 12.f);

void TitleScreen::UpdateProjUniforms() {
    auto & assets = GetGame().GetAssetMgr();
    auto & shadowProgram = assets.GetProgram<ShaderProgramId::Shadow>();
    auto & lightingProg = assets.GetProgram<ShaderProgramId::Base>();
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
}
    
void TitleScreen::UpdateState(SceneStack & state) {
    if (GetGame().GetInput().buttonSet->WeaponPressed()) {
	auto camera = std::make_unique<PlaneCamera>();
	camera->SetTarget(m_plane);
	m_plane->SetThrust(1.f);
	m_plane->SetPosition({15.f, 40.f, 15.f});
	m_plane->SetRotation({0.f, 0.f, 0.f});
	GetGame().SetCamera(std::move(camera));
	state.push(std::make_shared<WorldLoader>());
	m_plane.reset();
    }
}

bool TitleScreen::Display() {
    auto & game = GetGame();
    UpdateProjUniforms();
    game.DrawShadowMap();
    const auto & windowSize = game.GetWindowSize();
    glViewport(0, 0, windowSize.x, windowSize.y);
    glClearColor(0.0f, 0.0f, 0.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    auto & lightingProg =
        game.GetAssetMgr().GetProgram<ShaderProgramId::Base>();
    lightingProg.Use();
    const auto view = game.GetCamera().GetWorldView();
    auto invView = glm::inverse(view);
    glm::vec3 eyePos = invView * glm::vec4(0, 0, 0, 1);
    if (auto playerPlane = game.GetPlayer().GetPlane()) {
	lightingProg.SetUniformVec3("eyePos", eyePos);
	lightingProg.SetUniformInt("shadowMap", 1);
	lightingProg.SetUniformFloat("overrideColorAmount",
				     game.GetPlayer().GetPlane()->GetMixAmount());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, game.GetShadowMapTxtr());
	playerPlane->Display(lightingProg);
    } else {
	throw std::runtime_error("Here");
    }
    glDisable(GL_DEPTH_TEST);
    m_titleText.Display();
    m_startText.Display();
    glEnable(GL_DEPTH_TEST);
    return true;
}
}
