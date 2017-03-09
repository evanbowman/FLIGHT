#pragma once

#include <chrono>
#include <thread>

#include "Time.hpp"

namespace FLIGHT {
namespace GAMEFEEL {
void Pause(const Time dt);
bool WasPaused();
void Reset();
}
}
