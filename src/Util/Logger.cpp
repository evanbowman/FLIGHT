#include <FLIGHT/Util/Logger.hpp>
#include <ctime>

namespace FLIGHT {
    void Logger::SetTarget(std::ostream & target) {
        m_target = &target;
    }
    
    void Logger::Write(const std::string & message) {
#ifndef NDEBUG
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_start);
        m_target << duration.count() << "ms : " << message << std::endl;
#endif
    }
}
