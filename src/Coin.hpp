#pragma once

#include "Entity.hpp"

namespace FLIGHT {
class Coin : public Solid {
public:
    void Display(ShaderProgram & shader) override;
    void Update(const Time dt) override;
    void OnCollide(Solid & other) override;
    AABB GetAABB() override;
};   
}
