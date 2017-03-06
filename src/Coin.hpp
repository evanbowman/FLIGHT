#pragma once

#include <memory>

#include "Entity.hpp"
#include "Model.hpp"

namespace FLIGHT {
class Coin : public Solid {
    std::weak_ptr<Model> m_model;
    Time m_timer;
public:
    Coin(const glm::vec3 & position);
    void Display(ShaderProgram & shader) override;
    void Update(const Time dt) override;
    void OnCollide(Solid & other) override;
    AABB GetAABB() override;
};
}
