#include <FLIGHT/Core/Game.hpp>
#include <FLIGHT/Core/Player.hpp>

namespace FLIGHT {
Player::Player() : m_lerpPitch{}, m_lerpRoll{} {}

std::shared_ptr<Plane> Player::GetPlane() const { return m_plane.lock(); }

void Player::GivePlane(std::shared_ptr<Plane> plane) { m_plane = plane; }

void Player::Update(const Time dt) {
    if (auto planeSp = m_plane.lock()) {
        auto jsDir =
            Singleton<Game>::Instance().GetInput().joystick->GetDirection();
        const auto orientVec =
            jsDir *
            Singleton<Game>::Instance().GetInput().joystick->GetMagnitude() *
            40.f;
        glm::vec2 currentVec = {planeSp->GetPitch(), planeSp->GetRoll()};
        currentVec = MATH::lerp(orientVec, currentVec, 0.000005f * dt);
        planeSp->SetPitch(currentVec.x);
        planeSp->SetRoll(currentVec.y);
        while (auto msg = planeSp->PollMessages()) {
            switch (msg->GetId()) {
            case Message::Id::PickedUpCoin:
                GAMEFEEL::Pause(10000);
                break;

            case Message::Id::Death:
                planeSp->SetDeallocFlag();
                m_plane.reset();
                GAMEFEEL::Pause(50000);
                Singleton<Game>::Instance().RequestRestart();
                break;

            default:
                throw MessageError(msg->GetId());
            }
        }
    }
}
}
