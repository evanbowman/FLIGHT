#include "../Game.hpp"
#include "../Scene.hpp"

namespace FLIGHT {
void TitleScreen::UpdateLogic(const Time dt) {
    // ...
}

void TitleScreen::UpdateState(SceneStack & state) {
    GetGame().GetTerrainMgr().SetSeed(time(nullptr));
    state.push(std::make_shared<WorldLoader>());
}

bool TitleScreen::Display() {
    // ...
    return true;
}
}
