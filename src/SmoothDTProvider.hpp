#pragma once

#include <array>

namespace FLIGHT {    
    class SmoothDTProvider {
	std::array<int64_t, 100> m_smoothingBuffer;
	size_t m_smoothingTracker;
    public:
	SmoothDTProvider();
	void Feed(const int64_t dt);
	int64_t Get() const;
    };
}
