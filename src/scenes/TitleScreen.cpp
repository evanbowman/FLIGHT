#include "../Game.hpp"
#include "../Scene.hpp"

namespace FLIGHT {
void TitleScreen::UpdateLogic(const Time dt) {
    // ...
}

void TitleScreen::UpdateState(SceneStack & state) {
    auto camera = std::make_unique<PlaneCamera>();
    auto startPlane = GetGame().CreateSolid<Plane>(GetGame().GetPlaneRegistry()["RedTail"]);
    startPlane->SetPosition({15.f, 40.f, 15.f});
    GetGame().GetPlayer().GivePlane(startPlane);
    camera->SetTarget(startPlane);
    GetGame().SetCamera(std::move(camera));
    state.push(std::make_shared<WorldLoader>());
}

bool TitleScreen::Display() {
    // ...
    return true;
}
}
