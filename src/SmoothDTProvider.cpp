#include "SmoothDTProvider.hpp"

namespace FLIGHT {
SmoothDTProvider::SmoothDTProvider()
    : m_smoothingBuffer{}, m_smoothingTracker(0) {}

void SmoothDTProvider::Feed(const int64_t dt) {
    m_smoothingBuffer[m_smoothingTracker++ % m_smoothingBuffer.size()] = dt;
}

int64_t SmoothDTProvider::Get() const {
    int64_t aggregate = 0;
    for (auto dt : m_smoothingBuffer) {
        aggregate += dt;
    }
    return aggregate / m_smoothingBuffer.size();
}
}
