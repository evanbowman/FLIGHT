#pragma once

#include <FLIGHT/Core/Blueprint.hpp>
#include <FLIGHT/Core/ColorMix.hpp>
#include <FLIGHT/Entity/Entity.hpp>
#include <FLIGHT/Core/GameMath.hpp>
#include <FLIGHT/Graphics/Sprite.hpp>
#include <FLIGHT/Core/TerrainManager.hpp>
#include <FLIGHT/Util/Time.hpp>

namespace FLIGHT {
class Plane : public DynamicSolidPreallocMBS,
	      public ColorMixDecay {
protected:
    std::string m_srcBlueprint;
    std::vector<Sprite> m_parts;
    glm::vec3 m_direction;
    float m_pitch;
    float m_roll;
    float m_thrust;
    float m_yVelocity;
    void MessageLoop();

public:
    constexpr static float GetElevationLimit() { return 50.f; }
    Plane(const Blueprint & blueprint, const std::string & blueprintName);
    const std::string & GetBlueprintName() const;
    const std::vector<Sprite> & GetParts() const;
    void Display(DisplayImpl & renderer) override;
    void Update(const Time dt) override;
    void Serialize(Serializer & serializer) override;
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
    [[deprecated]] void CastShadow(ShaderProgram & prog);
};
}
