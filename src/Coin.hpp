#pragma once

#include <memory>

#include "Entity.hpp"
#include "Model.hpp"
#include "Random.hpp"
#include "Util.hpp"

namespace FLIGHT {
class Coin : public StaticSolidPreallocMBS {
    std::weak_ptr<Model> m_model;
    Time m_timer;
    void MessageLoop();

public:
    Coin(const glm::vec3 & position);
    void Display(ShaderProgram & shader) override;
    void Update(const Time dt) override;
    AABB GetAABB() override;
    OBB GetOBB() override;
};
}
