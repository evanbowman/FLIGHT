#include "Player.hpp"

Player::Player(unsigned uid) : m_lerpPitch{}, m_lerpRoll{}, m_uid(uid) {}

std::shared_ptr<Plane> Player::GetPlane() {
    return m_plane;
}

void Player::GivePlane(std::shared_ptr<Plane> plane) {
    m_plane = plane;
}

void Player::Update() {
    auto planePos = m_plane->GetPosition();
    planePos.z -= 0.005;
    m_plane->SetPosition(planePos);
    float pitch = m_plane->GetPitch();
    float roll = m_plane->GetRoll();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
	if (roll < 40.f) {
	    roll += 0.1f;
	}
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
	if (roll > -40.f) {
	    roll -= 0.1f;
	}
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
	if (pitch < 40.f) {
	    pitch += 0.1f;
	}
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
	if (pitch > -40.f) {
	    pitch -= 0.1f;
	}
    }
    m_plane->SetPitch(pitch);
    m_plane->SetRoll(roll);
    m_lerpPitch = math::lerp(pitch, m_lerpPitch, 0.01);
    m_lerpRoll = math::lerp(roll, m_lerpRoll, 0.01);
    m_plane->Update();
    auto rot = m_plane->GetRotation();
    m_plane->SetRotation({
	    glm::radians(m_lerpPitch), rot.y, glm::radians(m_lerpRoll)
	});
}
