#include <FLIGHT/Core/Game.hpp>
#include <FLIGHT/Core/Player.hpp>

namespace FLIGHT {
Player::Player() : m_lerpPitch{}, m_lerpRoll{}, m_score(0), m_lerpScore(0) {
    m_powerups[0] = Powerup::Speed;
    m_powerups[1] = Powerup::Shield;
    m_powerups[2] = Powerup::Cloak;
}

std::shared_ptr<Plane> Player::GetPlane() const { return m_plane.lock(); }

void Player::GiveController(std::unique_ptr<Controller> controller) {
    m_controller = std::move(controller);
}

Controller * Player::GetController() { return m_controller.get(); }

std::unique_ptr<Controller> Player::TakeController() {
    return std::move(m_controller);
}

void Player::GivePlane(std::shared_ptr<Plane> plane) { m_plane = plane; }

Score Player::GetScore() const { return m_score; }

float Player::GetDelayedScore() const { return m_lerpScore; }

void Player::SetScore(const Score score) { m_score = score; }

void Player::Update(const Time dt) {
    if (auto planeSp = m_plane.lock()) {
        if (m_controller) {
            auto jsDir = m_controller->GetJoystick().GetDirection();
            const auto orientVec =
                jsDir * m_controller->GetJoystick().GetMagnitude() * 40.f;
            glm::vec2 currentVec = {planeSp->GetPitch(), planeSp->GetRoll()};
            currentVec = MATH::lerp(orientVec, currentVec, 0.000005f * dt);
            planeSp->SetPitch(currentVec.x);
            planeSp->SetRoll(currentVec.y);
        } else {
            planeSp->SetPitch(0.f);
            planeSp->SetRoll(0.f);
        }
        m_lerpScore = MATH::lerp((float)m_score, m_lerpScore, 0.000005f * dt);
        auto & game = Singleton<Game>::Instance();
        planeSp->PollMessages([this, &planeSp, &game](auto & msg) {
            msg.match(
                [this](PickedUpCoin) {
                    GAMEFEEL::Pause(10000);
                    m_score += 100;
                },
                [this, &planeSp, &game](Death) {
                    planeSp->SetDeallocFlag();
                    m_plane.reset();
                    GAMEFEEL::Pause(50000);
                    this->SetScore(0);
                    game.RemoveSaveData();
                    game.RequestRestart();
                },
                [](auto &) { throw MessageError(); });
        });
    }
}

Player::PowerupList & Player::GetPowerups() { return m_powerups; }
}
