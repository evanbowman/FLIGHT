#pragma once

#include <FLIGHT/Graphics/Color.hpp>
#include <FLIGHT/Graphics/Model.hpp>
#include <FLIGHT/Entity/Entity.hpp>

namespace FLIGHT {
    class Thruster : public Entity {
	Color m_color;
	std::weak_ptr<Model> m_model;
    public:
	Thruster();
	void Update(const Time dt) override {}
	void SetColor(const Color & color);
	void Display(DisplayDispatcher & dispatcher) override;
    };
}
