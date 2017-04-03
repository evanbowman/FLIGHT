#include <FLIGHT/Entity/Effect/Thruster.hpp>
#include <FLIGHT/Core/Game.hpp>

namespace FLIGHT {
    Thruster::Thruster() {
	m_model = GetGame().GetAssetMgr().GetModel("Thruster.obj");
    }

    void Thruster::Display(DisplayImpl & gfx) {
	if (auto modelSp = m_model.lock()) {
	    // glm::mat4 mdl = glm::translate(mdl, m_position);
	    // shader.SetUniformMat4("model", mdl);
	    // auto binding = modelSp->Bind(shader);
	    // glDrawArrays(GL_TRIANGLES, 0, binding.numVertices);
	    gfx.Dispatch(*this);
	}
    }

    void Thruster::SetColor(const Color & color) {
	m_color = color;
    }
}
