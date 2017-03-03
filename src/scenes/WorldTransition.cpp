#include "../Game.hpp"
#include "../Scene.hpp"

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

    bool WorldTransitionIn::Display() {
	World::Display();
	const float overlayDarkness =
	    1.f - glm::smoothstep(0.f, static_cast<float>(TRANSITION_TIME),
				  static_cast<float>(m_transitionTimer));
	DisplayShadowOverlay(overlayDarkness);
	return true;
    }
}
