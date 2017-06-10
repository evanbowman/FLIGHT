#pragma once

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <array>
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
    virtual unsigned * GetId() = 0;
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
    unsigned * GetId() override { return nullptr; }
    MouseJoystickProxy * Clone() override;
    void Update(const sf::Event & event) override;
};

class GamepadJoystick : public Joystick {
    unsigned m_id;

public:
    GamepadJoystick(const unsigned m_id);
    unsigned * GetId() override;
    void Update(const sf::Event & event) override;
    GamepadJoystick * Clone() override;
};

class ButtonSet {
protected:
    std::array<bool, 3> m_regPressed;
    std::array<bool, 3> m_regTapped;
    std::array<bool, 3> m_specialPressed;
    std::array<bool, 3> m_specialTapped;
    void ResetTaps();

public:
    void Reset();
    ButtonSet()
        : m_regPressed{}, m_regTapped{}, m_specialPressed{}, m_specialTapped{} {
    }
    template <size_t index> bool SpecialPressed() {
        return std::get<index>(m_specialPressed);
    }
    template <size_t index> bool SpecialTapped() {
        return std::get<index>(m_specialTapped);
    }
    template <size_t index> bool RegularPressed() {
        return std::get<index>(m_regPressed);
    }
    template <size_t index> bool RegularTapped() {
        return std::get<index>(m_regTapped);
    }
    virtual void Update(const sf::Event & event) = 0;
    virtual ~ButtonSet() {}
};

class KeyboardButtonSet : public ButtonSet {
    std::array<sf::Keyboard::Key, 3> m_regularMapping;
    std::array<sf::Keyboard::Key, 3> m_specialMapping;

public:
    KeyboardButtonSet(
        const ConfigData::ControlsConf::KeyboardMapping & mapping);
    void Update(const sf::Event & event) override;
};

class GamepadButtonSet : public ButtonSet {
    std::array<unsigned, 3> m_regularMapping;
    std::array<unsigned, 3> m_specialMapping;
    unsigned m_id;

public:
    GamepadButtonSet(const ConfigData::ControlsConf::GamepadMapping & mapping,
                     const unsigned id);
    void Update(const sf::Event & event) override;
};

// using Controller = std::pair<std::unique_ptr<Joystick>,
// 			     std::unique_ptr<ButtonSet>>;

class Controller {
public:
    virtual Joystick & GetJoystick() = 0;
    virtual ButtonSet & GetButtonSet() = 0;
    virtual unsigned * GetId() = 0;
};

class KeyboardMouseInput : public Controller {
    KeyboardButtonSet m_buttonSet;
    MouseJoystickProxy m_joystick;

public:
    KeyboardMouseInput(
        const ConfigData::ControlsConf::KeyboardMapping & mapping);
    Joystick & GetJoystick() override;
    ButtonSet & GetButtonSet() override;
    unsigned * GetId() override { return nullptr; }
};

class GamepadInput : public Controller {
    GamepadButtonSet m_buttonSet;
    GamepadJoystick m_joystick;

public:
    GamepadInput(const ConfigData::ControlsConf::GamepadMapping & mapping,
                 const unsigned id);
    Joystick & GetJoystick() override;
    ButtonSet & GetButtonSet() override;
    unsigned * GetId() override;
};
}
