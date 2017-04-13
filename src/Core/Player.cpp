#include <FLIGHT/Core/Game.hpp>
#include <FLIGHT/Core/Player.hpp>

namespace FLIGHT {
Player::Player() : m_lerpPitch{}, m_lerpRoll{}, m_score(0) {}

std::shared_ptr<Plane> Player::GetPlane() const { return m_plane.lock(); }

void Player::GivePlane(std::shared_ptr<Plane> plane) {
    m_plane = plane;
    this->SetScore(0);
}

Score Player::GetScore() const { return m_score; }

void Player::SetScore(const Score score) { m_score = score; }

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
                m_score += 10;
                break;

            case Message::Id::Death:
                planeSp->SetDeallocFlag();
                m_plane.reset();
                GAMEFEEL::Pause(50000);
                throw std::runtime_error(std::to_string(m_score));
                Singleton<Game>::Instance().RequestRestart();
                break;

            default:
                throw MessageError(msg->GetId());
            }
        }
    }
}
}
