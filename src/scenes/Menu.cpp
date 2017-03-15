#include "../Game.hpp"
#include "../Scene.hpp"

namespace FLIGHT {
Menu::Menu() {
    std::array<std::string, 3> options = {"Resume", "Settings", "Quit"};
    for (const auto & option : options) {
        auto tv = std::make_unique<UI::TextView>();
        tv->GetText().SetFace(
            GetGame().GetAssetMgr().GetFontFace("MuseoSlab700.ttf"));
        tv->GetText().SetString(option);
	tv->SetMargin({0, 0, 20, 0});
        m_layout.AddChild(std::move(tv));
    }
    const auto & windowSize = GetGame().GetWindowSize();
    m_layout.SetPosition({0.f, windowSize.y});
    m_layout.SetMargin({15, 15, 15, 15});
}

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
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    GetGame().DisplayStash();
    m_layout.Display();
    glEnable(GL_DEPTH_TEST);
    return true;
}
}
