#pragma once

#include <FLIGHT/Entity/Entity.hpp>
#include <FLIGHT/Graphics/Color.hpp>
#include <FLIGHT/Graphics/Model.hpp>

namespace FLIGHT {
class Thruster : public Entity {
    Color m_color;
    std::weak_ptr<Model> m_model;

public:
    Thruster();
    void Update(const Time dt) override {}
    void SetColor(const Color & color);
    void Display(DisplayImpl & renderer) override;
};
}
