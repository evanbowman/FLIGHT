#include "../Scene.hpp"
#include "../Game.hpp"

void Menu::UpdateLogic(const Time dt) {
    // ...
}

void Menu::UpdateState(SceneStack & state) {
    if (GetGame().GetInput().buttonSet->PausePressed()) {
        state.pop();
	state.push(std::make_shared<MenuTransitionOut>());
    }
}

bool Menu::Display() {
    // ...
    return false;
}
