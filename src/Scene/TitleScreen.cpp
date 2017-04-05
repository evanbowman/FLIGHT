#include <FLIGHT/Core/Game.hpp>
#include <FLIGHT/Scene/Scene.hpp>

namespace FLIGHT {
TitleScreen::TitleScreen()
    : m_timer(0), m_transitionTimer(0), m_state(State::ShowTitle) {
    auto & game = Singleton<Game>::Instance();
    const auto & windowSize = game.GetWindowSize();
    m_titleText.SetFace(game.GetAssetMgr().GetFont<FontSize::Large>());
    m_startText.SetFace(game.GetAssetMgr().GetFont<FontSize::Small>());
    m_titleText.SetString(game.GetConf().localization.strings.appName);
    m_startText.SetString(game.GetConf().localization.strings.startText);
    m_startText.SetColor({0.f, 0.f, 0.1f, 0.5f});
    m_titleText.SetColor({0.f, 0.f, 0.1f, 1.f});
    m_titleText.SetPosition({windowSize.x / 2 - m_titleText.GetSize().x / 2.f,
                             windowSize.y - 2.5 * m_titleText.GetSize().y,
                             0.f});
    m_startText.SetPosition({windowSize.x / 2 - m_startText.GetSize().x / 2.f,
                             4 * m_startText.GetSize().y, 0.f});
    auto camera = std::unique_ptr<Camera>(new ElasticCamera(glm::vec3{-0.5, 0.2, -2}));
    m_plane =
        game.CreateSolid<Plane>(game.GetPlaneRegistry()["RedTail"]);
    m_plane->SetThrust(0.f);
    m_plane->SetRotation({0.15, 0, 0.433});
    game.GetPlayer().GivePlane(m_plane);
    camera->SetTarget(m_plane);
    game.SetCamera(std::move(camera));
}

static const Time ENTRY_TRANSITION_TIME(550000);

void TitleScreen::UpdateLogic(const Time dt) {
    auto & game = Singleton<Game>::Instance();
    switch (m_state) {
    case State::ShowTitle:
        m_timer += dt;
        if (m_transitionTimer < ENTRY_TRANSITION_TIME) {
            m_transitionTimer += dt;
            float textAnimFactor = glm::smoothstep(
                0.f, (float)ENTRY_TRANSITION_TIME, (float)m_transitionTimer);
            const auto & windowSize = game.GetWindowSize();
            m_titleText.SetPosition(
                {windowSize.x / 2 - m_titleText.GetSize().x / 2.f,
                 windowSize.y - 2.5 * m_titleText.GetSize().y * textAnimFactor,
                 0.f});
            m_startText.SetPosition(
                {windowSize.x / 2 - m_startText.GetSize().x / 2.f,
                 4 * m_startText.GetSize().y * textAnimFactor, 0.f});
            m_startText.SetColor({0.f, 0.f, 0.1f, textAnimFactor});
            m_titleText.SetColor({0.f, 0.f, 0.1f, textAnimFactor});
        }
        m_startText.SetColor(
            {0.f, 0.f, 0.1f,
             static_cast<float>(
                 1.f -
                 (1.f + std::sin(2 * UTIL::PI * 0.000001 * m_timer)) / 3.f)});
        if (game.GetInput().buttonSet->WeaponPressed()) {
            m_state = State::PlaneSelectorEntry;
            m_timer = 0;
            m_transitionTimer = 0;
            dynamic_cast<ElasticCamera &>(game.GetCamera())
                .SetPosition({0, 0.1, -3.3});
        }
        break;

    case State::PlaneSelectorEntry: {
        auto rot = m_plane->GetRotation();
        if (std::abs(glm::length(rot - glm::vec3(0.f, 0.f, 0.f))) > 0.0002 ||
            m_transitionTimer < 440000) {
            m_transitionTimer += dt;
            static const float LERP_RATE = dt * 0.000005f;
            rot = glm::mix(rot, glm::vec3(0.f, 0.f, 0.f), LERP_RATE);
            m_plane->SetRotation(rot);
            float textAnimFactor = glm::smoothstep(
                (float)ENTRY_TRANSITION_TIME, 0.f, (float)m_transitionTimer);
            const auto & windowSize = game.GetWindowSize();
            m_titleText.SetPosition(
                {windowSize.x / 2 - m_titleText.GetSize().x / 2.f,
                 windowSize.y - 2.5 * m_titleText.GetSize().y * textAnimFactor,
                 0.f});
            m_startText.SetPosition(
                {windowSize.x / 2 - m_startText.GetSize().x / 2.f,
                 4 * m_startText.GetSize().y * textAnimFactor, 0.f});
            m_startText.SetColor({0.f, 0.f, 0.1f, textAnimFactor});
            m_titleText.SetColor({0.f, 0.f, 0.1f, textAnimFactor});
        } else {
            m_state = State::PlaneSelector;
            m_transitionTimer = 0;
        }
    } break;

    case State::PlaneSelector:
        break;
    }
    game.GetCamera().Update(dt);
}

static const glm::mat4 LIGHT_PROJ_MAT =
    glm::ortho(-4.f, 4.f, -4.f, 4.f, -5.f, 12.f);

void TitleScreen::UpdateProjUniforms() {
    auto & game = Singleton<Game>::Instance();
    auto & assets = game.GetAssetMgr();
    auto & shadowProgram = assets.GetProgram<ShaderProgramId::Shadow>();
    auto & lightingProg = assets.GetProgram<ShaderProgramId::Base>();
    shadowProgram.Use();
    auto & camera = game.GetCamera();
    auto view = camera.GetLightView();
    auto lightSpace = LIGHT_PROJ_MAT * view;
    shadowProgram.SetUniformMat4("lightSpace", lightSpace);

    lightingProg.Use();
    view = camera.GetWorldView();
    const auto & windowSize = game.GetWindowSize();
    const float aspect =
        static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);
    const glm::mat4 perspective = glm::perspective(45.0f, aspect, 0.1f, 1.0f);
    auto cameraSpace = perspective * view;
    lightingProg.SetUniformMat4("lightSpace", lightSpace);
    lightingProg.SetUniformMat4("cameraSpace", cameraSpace);
}

void TitleScreen::UpdateState(SceneStack & state) {
    auto & game = Singleton<Game>::Instance();
    if (m_state == State::PlaneSelector) {
        if (game.GetInput().buttonSet->WeaponPressed() &&
            m_transitionTimer < ENTRY_TRANSITION_TIME) {
            auto camera = std::unique_ptr<Camera>(new PlaneCamera);
            camera->SetTarget(m_plane);
            m_plane->SetThrust(1.f);
            m_plane->SetPosition({15.f, 40.f, 15.f});
            m_plane->SetRotation({0.f, 0.f, 0.f});
            game.SetCamera(std::move(camera));
            state.push(std::make_shared<WorldLoader>());
            m_plane.reset();
        }
    }
}

static void DisplayOverlay(const float amount) {
    auto & game = Singleton<Game>::Instance();
    auto & genericProg =
        game.GetAssetMgr().GetProgram<ShaderProgramId::Generic>();
    genericProg.Use();
    const auto windowSize = game.GetWindowSize();
    const glm::mat4 ortho = glm::ortho(0.f, static_cast<float>(windowSize.x),
                                       0.f, static_cast<float>(windowSize.y));
    genericProg.SetUniformMat4("cameraSpace", ortho);
    glm::mat4 model = glm::translate(glm::mat4(1), {0, windowSize.y, 0.f});
    model = glm::scale(model, {windowSize.x, windowSize.y, 0.f});
    PRIMITIVES::Quad quad;
    genericProg.SetUniformVec4("color", {0, 0, 0, amount});
    genericProg.SetUniformMat4("model", model);
    quad.Display(genericProg, AlphaBlend);
}

void DrawFloor() {}

void TitleScreen::Display(DisplayImpl & renderer) {
    renderer.Dispatch(*this);
}
}
