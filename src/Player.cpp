#include "Player.hpp"
#include "Game.hpp"

Player::Player(unsigned uid) : m_lerpPitch{}, m_lerpRoll{}, m_uid(uid) {}

std::shared_ptr<Plane> Player::GetPlane() {
    return m_plane;
}

void Player::GivePlane(std::shared_ptr<Plane> plane) {
    m_plane = plane;
}

void Player::Update(const Time dt) {
    auto jsDir = GetGame().GetInput().joystick->GetDirection();
    const auto orientVec = jsDir * GetGame().GetInput().joystick->GetMagnitude() * 40.f;
    glm::vec2 currentVec = {m_plane->GetPitch(), m_plane->GetRoll()};
    currentVec = math::lerp(orientVec, currentVec, 0.000005f * dt);
    m_plane->SetPitch(currentVec.x);
    m_plane->SetRoll(currentVec.y);
    m_plane->Update(dt);
}
