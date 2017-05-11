#pragma once

#include "GameFeel.hpp"
#include "GameMath.hpp"
#include <FLIGHT/Entity/Plane.hpp>
#include <FLIGHT/Util/Optional.hpp>
#include "InputModes.hpp"
#include "Powerup.hpp"

namespace FLIGHT {
class Player {
public:
    #define NUM_POWERUP_SLOTS 3
    using PowerupList = std::array<Powerup, NUM_POWERUP_SLOTS>;
    
private:
    mutable std::weak_ptr<Plane> m_plane;
    float m_lerpPitch, m_lerpRoll;
    Score m_score;
    float m_lerpScore;
    std::unique_ptr<Controller> m_controller;
    PowerupList m_powerups;
    void ReactToSpecKeyPresses(ButtonSet & buttonSet);
    
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
    // NOTE: The delayed score exists for purely aesthetic reasons,
    // allowing the score bars to slowly drift to a steady state when
    // changed.
    float GetDelayedScore() const;
    void SetScore(const Score score);
    PowerupList & GetPowerups();
};
}
