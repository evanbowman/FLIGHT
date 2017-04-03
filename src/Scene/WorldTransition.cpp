#include <FLIGHT/Core/Game.hpp>
#include <FLIGHT/Scene/Scene.hpp>

namespace FLIGHT {
void WorldTransitionIn::UpdateLogic(const Time dt) {
    World::UpdateLogic(dt);
    m_transitionTimer += dt;
}

void WorldTransitionIn::UpdateState(SceneStack & state) {
    if (m_transitionTimer > TRANSITION_TIME) {
        state.pop();
        state.push(std::make_shared<World>());
    }
}

bool WorldTransitionIn::Display(DisplayDispatcher & dispatcher) {
    dispatcher.Dispatch(*this);
    return true;
}
}
