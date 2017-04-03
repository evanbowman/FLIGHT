#pragma once

#include <FLIGHT/Core/Blueprint.hpp>
#include <FLIGHT/Core/ColorMix.hpp>
#include <FLIGHT/Entity/Entity.hpp>
#include <FLIGHT/Core/GameMath.hpp>
#include <FLIGHT/Graphics/Sprite.hpp>
#include <FLIGHT/Core/TerrainManager.hpp>
#include <FLIGHT/Core/Time.hpp>

namespace FLIGHT {
class Plane : public DynamicSolidPreallocMBS, public ColorMixDecay {
protected:
    std::vector<Sprite> m_parts;
    glm::vec3 m_direction;
    float m_pitch;
    float m_roll;
    float m_thrust;
    float m_yVelocity;
    void MessageLoop();

public:
    constexpr static float GetElevationLimit() { return 50.f; }
    Plane();
    Plane(const Blueprint & blueprint);
    const std::vector<Sprite> & GetParts() const;
    void Display(DisplayDispatcher & dispatcher) override;
    void Update(const Time dt) override;
    const glm::vec3 & GetDirection() const;
    void SetDirection(const glm::vec3 & direction);
    void SetPitch(const float);
    float GetPitch() const;
    void SetRoll(const float);
    float GetRoll() const;
    void SetThrust(const float);
    float GetThrust() const;
    AABB GetAABB() override;
    OBB GetOBB() override;
};
}
