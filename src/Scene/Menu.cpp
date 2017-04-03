#include <FLIGHT/Core/Game.hpp>
#include <FLIGHT/Scene/Scene.hpp>

namespace FLIGHT {
Menu::Menu() {
    std::array<std::string, 4> options = {"Resume", "Options", "Restart",
                                          "Quit"};
    for (const auto & option : options) {
        auto tv = std::unique_ptr<UI::TextView>(new UI::TextView);
        tv->GetText().SetFace(
            GetGame().GetAssetMgr().GetFont<FontSize::Medium>());
        tv->GetText().SetString(option);
        tv->SetMargin({0, 0, 30, 0});
        m_layout.AddChild(std::move(tv));
    }
    const auto & windowSize = GetGame().GetWindowSize();
    m_layout.SetPosition({0.f, windowSize.y});
    m_layout.SetMargin({15, 15, 15, 15});
}

void Menu::UpdateLogic(const Time dt) { m_layout.Update(dt); }

void Menu::UpdateState(SceneStack & state) {
    if (GetGame().GetInput().buttonSet->PausePressed()) {
        state.pop();
        state.push(std::make_shared<MenuTransitionOut>());
    }
}

bool Menu::Display(DisplayDispatcher & dispatcher) {
    dispatcher.Dispatch(*this);
    return true;
}
}
