#pragma once

#include <glm/glm.hpp>

#include "GameMath.hpp"
#include "Time.hpp"

namespace FLIGHT {
class ColorMix {
protected:
    glm::vec4 m_color{};
    float m_mixAmt = 0.f;

public:
    const glm::vec4 & GetColor() const { return m_color; }
    void SetColor(const glm::vec4 & color) { m_color = color; }
    float GetMixAmount() const { return m_mixAmt; }
};

class ColorMixDecay : public ColorMix {
    Time m_timer = 0;

public:
    void Update(const Time dt) {
        if (m_mixAmt > 0.f) {
            m_timer += dt;
            m_mixAmt = MATH::lerp(0.f, 1.f, m_timer / 800000.f);
        }
    }
    void BeginDecay() {
        m_timer = 0;
        m_mixAmt = 1.f;
    }
};
}
