#include "../Scene.hpp"
#include "../Game.hpp"

void TitleScreen::UpdateLogic(const Time dt) {
    // ...
}

void TitleScreen::UpdateState(SceneStack & state) {
    GetGame().GetTerrain().SetSeed(time(nullptr));
    state.push(std::make_unique<WorldLoader>());
}

void TitleScreen::Display() {
    // ...
}
