#pragma once

#include <array>
#include <cassert>

namespace FLIGHT {
using Score = unsigned long;

enum class Powerup : unsigned {
    None, Speed, Shield, Cloak, Count
};


Score GetCost(const Powerup powerup);
}
