#pragma once

#include <array>

enum class Powerup {
    None
};

#define NUM_POWERUP_SLOTS 3
using PowerupList = std::array<Powerup, NUM_POWERUP_SLOTS>;
