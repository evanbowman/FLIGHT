#include <FLIGHT/Core/Game.hpp>
#include <FLIGHT/Scene/Scene.hpp>

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

void MenuTransitionIn::Display(DisplayDispatcher & dispatcher) {
    dispatcher.Dispatch(*this);
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

void MenuTransitionOut::Display(DisplayDispatcher & dispatcher) {
    dispatcher.Dispatch(*this);
}
}
