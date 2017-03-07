#include "GameFeel.hpp"

namespace FLIGHT {
    namespace GAMEFEEL {
	static bool g_wasPaused;
	
	void Pause(const Time dt) {
	    std::this_thread::sleep_for(std::chrono::microseconds(dt));
	    g_wasPaused = true;
	}

	bool WasPaused() {
	    return g_wasPaused;
	}

	void Reset() {
	    g_wasPaused = false;
	}
    }
}
