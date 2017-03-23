#pragma once

#include "GameFeel.hpp"
#include "GameMath.hpp"
#include <FLIGHT/Entity/Plane.hpp>

namespace FLIGHT {
class Player {
    mutable std::weak_ptr<Plane> m_plane;
    float m_lerpPitch, m_lerpRoll;

public:
    Player();
    void GivePlane(std::shared_ptr<Plane> plane);
    void DisposePlane();
    std::shared_ptr<Plane> GetPlane() const;
    void Update(const Time dt);
};
}
