#include "../Game.hpp"
#include "../Scene.hpp"

namespace FLIGHT {
void TitleScreen::UpdateLogic(const Time dt) {
    // ...
}

void TitleScreen::UpdateState(SceneStack & state) {
    state.push(std::make_shared<WorldLoader>());
}

bool TitleScreen::Display() {
    // ...
    return true;
}
}
