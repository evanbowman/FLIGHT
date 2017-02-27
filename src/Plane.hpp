#pragma once

#include "Entity.hpp"
#include "Sprite.hpp"
#include "AssetManager.hpp"
#include "Time.hpp"
#include "GameMath.hpp"
#include "TerrainManager.hpp"

class Plane : public Solid {
protected:
    std::vector<Sprite> m_components;
    glm::vec3 m_direction;
    float m_pitch;
    float m_roll;
    float m_thrust;
    float m_yVelocity;
public:
    constexpr static float GetElevationLimit() {
	return 80.f;
    }
    Plane();
    void Display(ShaderProgram & shader) override;
    void Update(const Time dt) override;
    const glm::vec3 & GetDirection() const;
    void SetDirection(const glm::vec3 & direction);
    void SetPitch(const float);
    float GetPitch() const;
    void SetRoll(const float);
    float GetRoll() const;
    void SetThrust(const float);
    float GetThrust() const;
    void OnCollide(Solid & other) override;
    AABB GetAABB() override;
};
