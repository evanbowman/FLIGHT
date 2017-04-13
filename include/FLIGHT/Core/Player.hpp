#pragma once

#include "GameFeel.hpp"
#include "GameMath.hpp"
#include <FLIGHT/Entity/Plane.hpp>

namespace FLIGHT {
using Score = unsigned long;
    
class Player {
    mutable std::weak_ptr<Plane> m_plane;
    float m_lerpPitch, m_lerpRoll;
    Score m_score;

public:
    Player();
    void GivePlane(std::shared_ptr<Plane> plane);
    void DisposePlane();
    std::shared_ptr<Plane> GetPlane() const;
    void Update(const Time dt);
    Score GetScore() const;
    void SetScore(const Score score);
};
}
