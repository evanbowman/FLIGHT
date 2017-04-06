#include <FLIGHT/Core/Game.hpp>
#include <FLIGHT/Entity/Effect/Thruster.hpp>

namespace FLIGHT {
Thruster::Thruster() {
    m_model =
        Singleton<Game>::Instance().GetAssetMgr().GetModel("Thruster.obj");
}

void Thruster::Display(DisplayImpl & renderer) {
    if (auto modelSp = m_model.lock()) {
        renderer.Dispatch(*this);
    }
}

void Thruster::SetColor(const Color & color) { m_color = color; }
}
