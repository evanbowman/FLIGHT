#include "../Scene.hpp"
#include "../Game.hpp"

void Menu::UpdateLogic(const Time dt) {
    // ...
}

void Menu::UpdateState(SceneStack & state) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        state.pop();
	state.push(std::make_unique<MenuTransitionOut>());
    }
}

bool Menu::Display() {
    // ...
    return false;
}
