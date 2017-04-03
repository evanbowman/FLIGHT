#include <FLIGHT/Core/Game.hpp>
#include <FLIGHT/Scene/Scene.hpp>

namespace FLIGHT {
CreditsScreen::CreditsScreen() : m_timer(0), m_state(State::BeginDelay) {
    auto & game = GetGame();
    m_text.SetFace(game.GetAssetMgr().GetFont<FontSize::Medium>());
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
        if (m_timer > 200000) {
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
	auto plane = GetGame().CreateSolid<Plane>(GetGame().GetPlaneRegistry()["RedTail"]);
	auto camera = std::unique_ptr<Camera>(new PlaneCamera);
	camera->SetTarget(plane);
	GetGame().SetCamera(std::move(camera));
	GetGame().GetPlayer().GivePlane(plane);
	plane->SetPosition({15.f, 40.f, 15.f});
	state.push(std::make_shared<WorldLoader>());
    }
}

void CreditsScreen::Display(DisplayImpl & gfx) {
    if (m_state == State::Enter || m_state == State::Exit) {
	gfx.Dispatch(*this);
    }
}
}
