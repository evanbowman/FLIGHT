#pragma once

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <glm/glm.hpp>
#include <utility>

#include "ConfigData.hpp"

namespace FLIGHT {
class Joystick {
protected:
    glm::vec2 m_direction{};
    float m_magnitude = 0;

public:
    void Zero();
    inline glm::vec2 GetDirection() const { return m_direction; }
    inline float GetMagnitude() const { return m_magnitude; }
    virtual void Update(const sf::Event & event) = 0;
    virtual Joystick * Clone() = 0;
    virtual ~Joystick() {}
};

struct Circle {
    glm::vec2 center;
    float radius;
};

class MouseJoystickProxy : public Joystick {
    Circle m_circle;
    size_t m_yields;
    float m_sensitivity;
    size_t InputYield();

public:
    MouseJoystickProxy();
    MouseJoystickProxy * Clone() override;
    void Update(const sf::Event & event) override;
};

class GamepadJoystick : public Joystick {
public:
    void Update(const sf::Event & event) override;
    GamepadJoystick * Clone() override;
};

class ButtonSet {
protected:
    bool m_pausePressed, m_weaponPressed, m_aimPressed;

public:
    ButtonSet()
        : m_pausePressed(false), m_weaponPressed(false), m_aimPressed(false) {}
    bool PausePressed() const;
    bool WeaponPressed() const;
    bool AimPressed() const;
    virtual void Update(const sf::Event & event) = 0;
    virtual ~ButtonSet() {}
};

class KeyboardButtonSet : public ButtonSet {
    sf::Keyboard::Key m_pauseMapping, m_weaponMapping, m_aimMapping;

public:
    KeyboardButtonSet(
        const ConfigData::ControlsConf::KeyboardMapping & mapping);
    void Update(const sf::Event & event) override;
};

class GamepadButtonSet : public ButtonSet {
    unsigned m_pauseMapping, m_weaponMapping, m_aimMapping;

public:
    GamepadButtonSet(const ConfigData::ControlsConf::GamepadMapping & mapping);
    void Update(const sf::Event & event) override;
};
}
