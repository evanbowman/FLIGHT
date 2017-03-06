#include "Coin.hpp"
#include "Plane.hpp"

namespace FLIGHT {
void Coin::Display(ShaderProgram & shader) {
    // ...
}

void Coin::Update(const Time dt) {}

void Coin::OnCollide(Solid & other) {
    if (dynamic_cast<Plane *>(&other)) {
        SetDeallocFlag();
    }
}

AABB Coin::GetAABB() { return {}; }
}
