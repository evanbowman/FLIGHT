#include "InputModes.hpp"

MouseJoystickProxy::MouseJoystickProxy() : m_yields(0), m_sensitivity(100.f) {
    auto dm = sf::VideoMode::getDesktopMode();
    m_circle.radius = m_sensitivity;
    m_circle.center.x = dm.width / 2;
    m_circle.center.y = dm.height / 2;
}

size_t MouseJoystickProxy::Yield() {
    if (m_yields > 0) {
	return m_yields--;
    } else {
	return 0;
    }
}

void MouseJoystickProxy::Update(const sf::Event::MouseMoveEvent & event) {
    const glm::vec2 coord{event.x, event.y};
    const float displ = std::abs(glm::length(coord - m_circle.center));
    m_direction.y = -(coord.x - m_circle.center.x);
    m_direction.x = coord.y - m_circle.center.y;
    m_magnitude = std::min(glm::length(m_direction), m_circle.radius) / m_circle.radius;
    if (m_direction.x != 0.f || m_direction.y != 0.f) {
	m_direction = glm::normalize(m_direction);
    }
    if (displ > m_sensitivity) {
	const auto clampedPos = m_circle.center + glm::normalize(glm::vec2 {
		coord.x - m_circle.center.x, coord.y - m_circle.center.y
	    }) * m_circle.radius;
	sf::Mouse::setPosition({static_cast<int>(clampedPos.x),
				static_cast<int>(clampedPos.y)});
	// Calling sf::Mouse::setPosition() triggers a mouse event, but
	// MouseProxy::Update() gets called from a mouse event handler. So
	// yeild and let it pass over.
	++m_yields;
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

MouseJoystickProxy * MouseJoystickProxy::Clone() {
    return new MouseJoystickProxy(*this);
}

Joystick * Joystick::Clone() {
    return new Joystick(*this);
}
