#pragma once

#include "Plane.hpp"
#include "GameMath.hpp"

namespace FLIGHT {
    class Player {
	mutable std::shared_ptr<Plane> m_plane;
	float m_lerpPitch, m_lerpRoll;
	unsigned m_uid;
    public:
	Player(unsigned uid);
	void GivePlane(std::shared_ptr<Plane> plane);
	std::shared_ptr<Plane> GetPlane() const;
	void Update(const Time dt);
    };
}
