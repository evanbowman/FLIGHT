#include <FLIGHT/Core/Player.hpp>
#include <FLIGHT/Core/Powerup.hpp>

namespace FLIGHT {
static const std::array<Score, static_cast<unsigned>(Powerup::Count)> g_costs{
    {0, 500, 1000, 700}};

Score GetCost(const Powerup powerup) {
#ifndef NDEBUG
    assert(static_cast<unsigned>(powerup) < g_costs.size());
#endif
    return g_costs[static_cast<unsigned>(powerup)];
}
}
