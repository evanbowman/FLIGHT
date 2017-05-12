#include <FLIGHT/Core/InputModes.hpp>

namespace FLIGHT {
void Joystick::Zero() {
    m_direction = glm::vec2{};
    m_magnitude = 0.f;
}

void ButtonSet::Reset() { ResetTaps(); }

void ButtonSet::ResetTaps() {
    for (auto & tap : m_regTapped) {
        tap = false;
    }
    for (auto & tap : m_specialTapped) {
        tap = false;
    }
}

MouseJoystickProxy::MouseJoystickProxy() : m_yields(0), m_sensitivity(100.f) {
    auto dm = sf::VideoMode::getDesktopMode();
    m_circle.radius = m_sensitivity;
    m_circle.center.x = dm.width / 2;
    m_circle.center.y = dm.height / 2;
    sf::Mouse::setPosition(
        {static_cast<int>(dm.width / 2), static_cast<int>(dm.height / 2)});
}

size_t MouseJoystickProxy::InputYield() {
    if (m_yields > 0) {
        return m_yields--;
    } else {
        return 0;
    }
}

void MouseJoystickProxy::Update(const sf::Event & event) {
    if (event.type == sf::Event::MouseMoved) {
        if (not InputYield()) {
            const glm::vec2 coord{event.mouseMove.x, event.mouseMove.y};
            const float displ = std::abs(glm::length(coord - m_circle.center));
            m_direction.y = -(coord.x - m_circle.center.x);
            m_direction.x = coord.y - m_circle.center.y;
            m_magnitude = std::min(glm::length(m_direction), m_circle.radius) /
                          m_circle.radius;
            if (m_direction.x not_eq 0.f or m_direction.y not_eq 0.f) {
                m_direction = glm::normalize(m_direction);
            }
            if (displ > m_sensitivity) {
                const auto clampedPos =
                    m_circle.center +
                    glm::normalize(glm::vec2{coord.x - m_circle.center.x,
                                             coord.y - m_circle.center.y}) *
                        m_circle.radius;
                sf::Mouse::setPosition({static_cast<int>(clampedPos.x),
                                        static_cast<int>(clampedPos.y)});
                // Calling sf::Mouse::setPosition() triggers a mouse event, but
                // MouseProxy::Update() gets called from a mouse event handler.
                // So
                // yeild and let it pass over.
                ++m_yields;
            }
        }
    }
}

unsigned * GamepadJoystick::GetId() { return &m_id; }

GamepadJoystick::GamepadJoystick(const unsigned id) : m_id(id) {}

void GamepadJoystick::Update(const sf::Event & event) {
    if (event.type == sf::Event::JoystickMoved &&
        event.joystickMove.joystickId == m_id) {
        static const int deadZone = 10;
        const float xTilt =
            sf::Joystick::getAxisPosition(m_id, sf::Joystick::Axis::X);
        const float yTilt =
            sf::Joystick::getAxisPosition(m_id, sf::Joystick::Axis::Y);
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
        if (m_direction.x not_eq 0.f or m_direction.y not_eq 0.f) {
            m_direction = glm::normalize(m_direction);
        }
    }
}

MouseJoystickProxy * MouseJoystickProxy::Clone() {
    return new MouseJoystickProxy(*this);
}

GamepadJoystick * GamepadJoystick::Clone() {
    return new GamepadJoystick(*this);
}

void KeyboardButtonSet::Update(const sf::Event & event) {
    switch (event.type) {
    case sf::Event::KeyPressed:
        if (event.key.code == m_regularMapping[0]) {
            m_regPressed[0] = true;
            m_regTapped[0] = true;
        } else if (event.key.code == m_regularMapping[1]) {
            m_regPressed[1] = true;
            m_regTapped[1] = true;
        } else if (event.key.code == m_regularMapping[2]) {
            m_regPressed[2] = true;
            m_regTapped[2] = true;
        } else if (event.key.code == m_specialMapping[0]) {
            m_specialPressed[0] = true;
            m_specialTapped[0] = true;
        } else if (event.key.code == m_specialMapping[1]) {
            m_specialPressed[1] = true;
            m_specialTapped[1] = true;
        } else if (event.key.code == m_specialMapping[2]) {
            m_specialPressed[2] = true;
            m_specialTapped[2] = true;
        }
        break;

    case sf::Event::KeyReleased:
        if (event.key.code == m_regularMapping[0]) {
            m_regPressed[0] = false;
        } else if (event.key.code == m_regularMapping[1]) {
            m_regPressed[1] = false;
        } else if (event.key.code == m_regularMapping[2]) {
            m_regPressed[2] = false;
        } else if (event.key.code == m_specialMapping[0]) {
            m_specialPressed[0] = false;
        } else if (event.key.code == m_specialMapping[1]) {
            m_specialPressed[1] = false;
        } else if (event.key.code == m_specialMapping[2]) {
            m_specialPressed[2] = false;
        }
        break;
    }
}

KeyboardButtonSet::KeyboardButtonSet(
    const ConfigData::ControlsConf::KeyboardMapping & mapping)
    : m_regularMapping(mapping.regularKeys),
      m_specialMapping(mapping.specialKeys) {}

void GamepadButtonSet::Update(const sf::Event & event) {
    switch (event.type) {
    case sf::Event::JoystickButtonPressed:
        if (event.joystickButton.joystickId != m_id) {
            return;
        }
        if (event.joystickButton.button == m_regularMapping[0]) {
            m_regPressed[0] = true;
            m_regTapped[0] = true;
        } else if (event.joystickButton.button == m_regularMapping[1]) {
            m_regPressed[1] = true;
            m_regTapped[1] = true;
        } else if (event.joystickButton.button == m_regularMapping[2]) {
            m_regPressed[2] = true;
            m_regTapped[2] = true;
        } else if (event.joystickButton.button == m_specialMapping[0]) {
            m_specialPressed[0] = true;
            m_specialTapped[0] = true;
        } else if (event.joystickButton.button == m_specialMapping[1]) {
            m_specialPressed[1] = true;
            m_specialTapped[1] = true;
        } else if (event.joystickButton.button == m_specialMapping[2]) {
            m_specialPressed[2] = true;
            m_specialTapped[2] = true;
        }
        break;

    case sf::Event::JoystickButtonReleased:
        if (event.joystickButton.joystickId != m_id) {
            return;
        }
        if (event.joystickButton.button == m_regularMapping[0]) {
            m_regPressed[0] = false;
        } else if (event.joystickButton.button == m_regularMapping[1]) {
            m_regPressed[1] = false;
        } else if (event.joystickButton.button == m_regularMapping[2]) {
            m_regPressed[2] = false;
        } else if (event.joystickButton.button == m_specialMapping[0]) {
            m_specialPressed[0] = false;
        } else if (event.joystickButton.button == m_specialMapping[1]) {
            m_specialPressed[1] = false;
        } else if (event.joystickButton.button == m_specialMapping[2]) {
            m_specialPressed[2] = false;
        }
        break;
    }
}

GamepadButtonSet::GamepadButtonSet(
    const ConfigData::ControlsConf::GamepadMapping & mapping, unsigned id)
    : m_regularMapping(mapping.regularKeys),
      m_specialMapping(mapping.specialKeys), m_id(id) {}

KeyboardMouseInput::KeyboardMouseInput(
    const ConfigData::ControlsConf::KeyboardMapping & mapping)
    : m_buttonSet(mapping) {}

unsigned * GamepadInput::GetId() { return m_joystick.GetId(); }

Joystick & KeyboardMouseInput::GetJoystick() { return m_joystick; }

ButtonSet & KeyboardMouseInput::GetButtonSet() { return m_buttonSet; }

Joystick & GamepadInput::GetJoystick() { return m_joystick; }

ButtonSet & GamepadInput::GetButtonSet() { return m_buttonSet; }

GamepadInput::GamepadInput(
    const ConfigData::ControlsConf::GamepadMapping & mapping, const unsigned id)
    : m_buttonSet(mapping, id), m_joystick(id) {}
}
