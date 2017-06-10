#pragma once

#include <memory>

#include <FLIGHT/Core/Util.hpp>
#include <FLIGHT/Entity/Entity.hpp>
#include <FLIGHT/Graphics/Model.hpp>
#include <FLIGHT/Util/Random.hpp>

namespace FLIGHT {
class Coin : public StaticSolidPreallocMBS {
    std::weak_ptr<Model> m_model;
    Time m_timer;
    void MessageLoop();

public:
    Coin(const glm::vec3 & position);
    void Display(DisplayImpl & renderer) override;
    void Update(const Time dt) override;
    void Serialize(Serializer & serializer) override;
    AABB GetAABB() override;
    OBB GetOBB() override;
};
}
