#include "../Game.hpp"
#include "../Scene.hpp"

namespace FLIGHT {
CreditsScreen::CreditsScreen() : m_timer(0), m_state(State::BeginDelay) {
    auto & game = GetGame();
    m_text.SetFace(game.GetAssetMgr().GetFont<FontSize::Large>());
    m_text.SetString(game.GetConf().localization.strings.credits);
    m_text.SetColor({1.f, 1.f, 1.f, 0.f});
    auto bounds = m_text.GetSize();
    const auto & windowSize = game.GetWindowSize();
    m_text.SetPosition({windowSize.x / 2 - bounds.x / 2,
                        windowSize.y / 2 - bounds.y / 2, 0.f});
}

void CreditsScreen::UpdateLogic(const Time dt) {
    m_timer += dt;
    static const Time ENTRY_TIME = 400000, WAIT_TIME = 1400000,
                      EXIT_TIME = 300000;
    switch (m_state) {
    case State::BeginDelay:
        if (m_timer > 50000) {
            m_timer = 0;
            m_state = State::Enter;
        }
        break;

    case State::Enter: {
        auto alpha = MATH::lerp(1.f, 0.f, m_timer / (float)ENTRY_TIME);
        m_text.SetColor({1.f, 1.f, 1.f, alpha});
    }
        if (m_timer > ENTRY_TIME) {
            m_state = State::Wait;
            m_text.SetColor({1.f, 1.f, 1.f, 1.f});
            m_timer = 0;
        }
        break;

    case State::Wait:
        if (m_timer > WAIT_TIME) {
            m_state = State::Exit;
            m_timer = 0;
        }
        break;

    case State::Exit: {
        auto alpha = MATH::lerp(0.f, 1.f, m_timer / (float)EXIT_TIME);
        m_text.SetColor({1.f, 1.f, 1.f, alpha});
    }
        if (m_timer > EXIT_TIME) {
            m_state = State::PostDelay;
            m_timer = 0;
        }
        break;

    case State::PostDelay:
        if (m_timer > 100000) {
            m_timer = 0;
            m_state = State::Done;
        }
        break;

    case State::Done:
        break;
    }
}

void CreditsScreen::UpdateState(SceneStack & state) {
    if (m_state == State::Done) {
        state.pop();
        state.push(std::make_shared<TitleScreen>());
    }
}

bool CreditsScreen::Display() {
    const auto & windowSize = GetGame().GetWindowSize();
    const glm::mat4 ortho = glm::ortho(0.f, static_cast<float>(windowSize.x),
                                       0.f, static_cast<float>(windowSize.y));
    auto & fontShader =
        GetGame().GetAssetMgr().GetProgram<ShaderProgramId::Font>();
    fontShader.Use();
    fontShader.SetUniformMat4("proj", ortho);
    glViewport(0, 0, windowSize.x, windowSize.y);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    m_text.Display();
    glEnable(GL_DEPTH_TEST);
    return true;
}
}
