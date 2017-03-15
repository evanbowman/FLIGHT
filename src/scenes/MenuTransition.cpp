#include "../Game.hpp"
#include "../Scene.hpp"

namespace FLIGHT {
MenuTransitionIn::MenuTransitionIn() : m_transitionTimer(0), m_done(false) {}

void MenuTransitionIn::UpdateLogic(const Time dt) {
    if (m_transitionTimer < TRANSITION_TIME) {
        World::UpdateLogic(dt);
        m_transitionTimer += dt;
    }
}

void MenuTransitionIn::UpdateState(SceneStack & state) {
    if (m_done) {
        state.pop();
        state.push(std::make_shared<Menu>());
    }
}

bool MenuTransitionIn::Display() {
    if (m_transitionTimer > TRANSITION_TIME) {
        m_transitionTimer = TRANSITION_TIME + 1;
    }
    World::Display();
    const float overlayDarkness =
        glm::smoothstep(0.f, static_cast<float>(TRANSITION_TIME),
                        static_cast<float>(m_transitionTimer) / 2.f);
    DisplayShadowOverlay(overlayDarkness);
    if (m_transitionTimer > TRANSITION_TIME) {
        GetGame().StashWindow();
        m_done = true;
    }
    return true;
}

MenuTransitionOut::MenuTransitionOut() : m_transitionTimer(0) {}

void MenuTransitionOut::UpdateLogic(const Time dt) {
    World::UpdateLogic(dt);
    m_transitionTimer += dt;
}

void MenuTransitionOut::UpdateState(SceneStack & state) {
    if (m_transitionTimer > TRANSITION_TIME) {
        state.pop();
    }
}

bool MenuTransitionOut::Display() {
    World::Display();
    const float overlayDarkness =
        0.5f - glm::smoothstep(0.f, static_cast<float>(TRANSITION_TIME),
                               static_cast<float>(m_transitionTimer) / 2.f);
    DisplayShadowOverlay(overlayDarkness);
    return true;
}
}
