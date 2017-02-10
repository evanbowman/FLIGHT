#pragma once

#include <SFML/Graphics.hpp>
#include <utility>
#include <glm/glm.hpp>
#include <algorithm>

class RotationProvider {
protected:
    glm::vec2 m_direction{};
    float m_magnitude = 0;
public:
    inline glm::vec2 GetDirection() const {
	return m_direction;
    }
    inline float GetMagnitude() const {
	return m_magnitude;
    }
    virtual ~RotationProvider() {}
};

struct Circle {
    glm::vec2 center;
    float radius;
};

// MouseRotProv uses the mouse to simulate a physical joystick
class MouseProxy : public RotationProvider {
    Circle m_circle;
    size_t m_yields;
    float m_sensitivity;
public:
    MouseProxy();
    void Update(const sf::Event::MouseMoveEvent & event);
    size_t Yield();
};

class Joystick : public RotationProvider {
public:
    void Update();
};

class ButtonProvider {
    // ...
};
