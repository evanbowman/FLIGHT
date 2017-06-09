#include <FLIGHT/Core/Game.hpp>
#include <FLIGHT/Entity/Coin.hpp>
#include <FLIGHT/Entity/Plane.hpp>

#include <iostream>

namespace FLIGHT {
static const double period = 0.0000005;

Coin::Coin(const glm::vec3 & position)
    : m_timer(rand() % (int)(1.0 / (2 * period))) {
    m_position = position;
    m_model = Singleton<Game>::Instance().GetAssetMgr().GetModel("Box.obj");
    m_mbsRadius = MBS(GetAABB()).GetRadius();
}

void Coin::Display(DisplayImpl & renderer) { renderer.Dispatch(*this); }

OBB Coin::GetOBB() { return OBB(GetAABB()); }

void Coin::Serialize(Serializer & serializer) { serializer.Dispatch(*this); }

void Coin::MessageLoop() {
    PollMessages([this](auto msg) {
        msg->match(
            [this](Collision c) {
                if (dynamic_cast<Plane *>(c.with.get())) {
                    SetDeallocFlag();
                }
            },
            [this](auto) { throw MessageError(); });
    });
}

void Coin::Update(const Time dt) {
    MessageLoop();
    m_timer += dt;
    const float offset = 0.0025 * sinf(2 * UTIL::PI * period * m_timer);
    m_position.y = m_position.y + offset;
}

AABB Coin::GetAABB() {
    return {{m_position.x - 0.5, m_position.y - 0.5, m_position.z - 0.5},
            {m_position.x + 1.5, m_position.y + 1.5, m_position.z + 1.5}};
}
}
