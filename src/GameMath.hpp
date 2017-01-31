#pragma once

namespace math {
    inline float lerp(const float A, const float B, const float t) {
	return A * t + (1 - t) * B;
    }
}
