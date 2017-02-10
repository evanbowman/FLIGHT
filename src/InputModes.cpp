#include "InputModes.hpp"

MouseProxy::MouseProxy() : m_yields(0) {
    auto dm = sf::VideoMode::getDesktopMode();
    m_box.width = 200;
    m_box.height = 200;
    m_box.top = dm.height / 2 - m_box.height / 2;
    m_box.left = dm.width / 2 - m_box.width / 2;
}

static void ClampToBox(std::pair<int, int> & pos, const sf::IntRect & box) {
    pos.first = std::min(std::max(box.left, pos.first), box.left + box.width);
    pos.second = std::min(std::max(box.top, pos.second), box.top + box.height);
}

size_t MouseProxy::Yield() {
    if (m_yields > 0) {
	return m_yields--;
    } else {
	return 0;
    }
}

void MouseProxy::Update(const sf::Event::MouseMoveEvent & event) {
    std::pair<int, int> coord{event.x, event.y};
    ClampToBox(coord, m_box);
    m_direction.y = -static_cast<float>(coord.first - (m_box.left + m_box.width / 2));
    m_direction.x = static_cast<float>(coord.second - (m_box.top + m_box.height / 2));
    const float boxRad = m_box.height / 2;
    m_magnitude = std::min(glm::length(m_direction), boxRad) / boxRad;
    if (m_direction.x != 0.f || m_direction.y != 0.f) {
	m_direction = glm::normalize(m_direction);
    }
}

void Joystick::Update() {
    static const int deadZone = 10;
    const float xTilt = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X);
    const float yTilt = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y);
    if (std::abs(xTilt) > deadZone) {
	m_direction.y = -xTilt;
    } else {
	m_direction.y = 0.f;
    }
    if (std::abs(yTilt) > deadZone) {
	m_direction.x = yTilt;
    } else {
	m_direction.x = 0.f;
    }
    m_magnitude = std::min(glm::length(m_direction), 100.f) / 100.f;
    if (m_direction.x != 0.f || m_direction.y != 0.f) {
	m_direction = glm::normalize(m_direction);
    }
}
