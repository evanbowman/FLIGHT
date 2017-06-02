#pragma once

#include <array>

namespace FLIGHT {
template <size_t width, size_t height>
class FloatMatrix {
    std::array<float, width * height> m_data = {};
public:
    inline float Get(const size_t row, const size_t col) const {
        return m_data[row * width + col];
    }

    void Set(const float value, const size_t row, const size_t col) {
        m_data[row * width + col] = value;
    }
};    
}
