#pragma once

namespace MATH {
    template <typename T>
    T lerp(const T & A, const T & B, const float t) {
	return A * t + (1 - t) * B;
    }
}
