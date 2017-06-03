#pragma once

#include <FLIGHT/Util/Time.hpp>

namespace FLIGHT {
namespace GAMEFEEL {
void Pause(const Time dt);
bool WasPaused();
void Reset();
}
}
