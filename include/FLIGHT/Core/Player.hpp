#pragma once

#include "GameFeel.hpp"
#include "GameMath.hpp"
#include <FLIGHT/Entity/Plane.hpp>
#include <FLIGHT/Util/Optional.hpp>
#include "InputModes.hpp"
#include "Powerup.hpp"

namespace FLIGHT {
using Score = unsigned long;
        
class Player {
    mutable std::weak_ptr<Plane> m_plane;
    float m_lerpPitch, m_lerpRoll;
    Score m_score;
    std::unique_ptr<Controller> m_controller;
    PowerupList m_powerups;
    
public:
    Player();
    void GiveController(std::unique_ptr<Controller> controller);
    Controller * GetController();
    std::unique_ptr<Controller> TakeController();
    void GivePlane(std::shared_ptr<Plane> plane);
    void DisposePlane();
    std::shared_ptr<Plane> GetPlane() const;
    void Update(const Time dt);
    Score GetScore() const;
    void SetScore(const Score score);
    PowerupList & GetPowerups();
};
}
