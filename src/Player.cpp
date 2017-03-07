#include "Player.hpp"
#include "Game.hpp"

namespace FLIGHT {
Player::Player(unsigned uid) : m_lerpPitch{}, m_lerpRoll{}, m_uid(uid) {}

std::shared_ptr<Plane> Player::GetPlane() const { return m_plane.lock(); }

void Player::GivePlane(std::shared_ptr<Plane> plane) { m_plane = plane; }

void Player::Update(const Time dt) {
    if (auto planeSp = m_plane.lock()) {
        auto jsDir = GetGame().GetInput().joystick->GetDirection();
        const auto orientVec =
            jsDir * GetGame().GetInput().joystick->GetMagnitude() * 40.f;
        glm::vec2 currentVec = {planeSp->GetPitch(), planeSp->GetRoll()};
        currentVec = MATH::lerp(orientVec, currentVec, 0.000005f * dt);
        planeSp->SetPitch(currentVec.x);
        planeSp->SetRoll(currentVec.y);
    }
}
}
