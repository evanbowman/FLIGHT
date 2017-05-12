#pragma once

#include <array>
#include <cassert>
#include <SFML/Graphics.hpp>

namespace FLIGHT {
using Score = unsigned long;

enum class Powerup : unsigned {
    None, Dash, Pulse, Heal, Count
};

Score GetCost(const Powerup powerup);

float CalcPowerupIconSize(const sf::Vector2u & windowSize);
}
