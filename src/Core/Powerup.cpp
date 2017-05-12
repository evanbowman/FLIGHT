#include <FLIGHT/Core/Player.hpp>
#include <FLIGHT/Core/Powerup.hpp>

namespace FLIGHT {
static const std::array<Score, static_cast<unsigned>(Powerup::Count)> g_costs{
    {0, 200, 500, 300}};

Score GetCost(const Powerup powerup) {
#ifndef NDEBUG
    assert(static_cast<unsigned>(powerup) < g_costs.size());
#endif
    return g_costs[static_cast<unsigned>(powerup)];
}

float CalcPowerupIconSize(const sf::Vector2u & windowSize) {
    return ((windowSize.x + windowSize.y) / 2.f) * 0.03f;
}
}
