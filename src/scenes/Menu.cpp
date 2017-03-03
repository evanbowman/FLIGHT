#include "../Game.hpp"
#include "../Scene.hpp"

namespace FLIGHT {
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
}
