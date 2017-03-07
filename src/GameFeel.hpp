#pragma once

#include <thread>
#include <chrono>

#include "Time.hpp"

namespace FLIGHT {
    namespace GAMEFEEL {
	void Pause(const Time dt);
	bool WasPaused();
	void Reset();
    }
}
