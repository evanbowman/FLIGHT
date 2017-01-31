#pragma once

#include "Plane.hpp"
#include "GameMath.hpp"

class Player {
    std::shared_ptr<Plane> m_plane;
    float m_lerpPitch, m_lerpRoll;
    unsigned m_uid;
public:
    Player(unsigned uid);
    void GivePlane(std::shared_ptr<Plane> plane);
    std::shared_ptr<Plane> GetPlane();
    void Update();
};
