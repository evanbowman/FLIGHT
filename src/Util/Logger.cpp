#include <FLIGHT/Util/Logger.hpp>
#include <ctime>

namespace FLIGHT {
    void Logger::Log(Logger::Priority level, const std::string & message) {
        if (static_cast<int>(level) >= static_cast<int>(m_level)) {
            if (m_out) {
                switch (level) {
                case Priority::Critical:
                    *m_out << "[CRITICAL]: ";
                    break;

                case Priority::Warn:
                    *m_out << "[WARN]: ";
                    break;

                case Priority::Info:
                    *m_out << "[INFO]: ";
                    break;

                case Priority::Debug:
                    *m_out << "[DEBUG]: ";
                    break;
                }
                std::lock_guard<std::mutex> lock(m_mutex);
                *m_out << message << std::endl;
            }
        }
    }

    void Logger::SetLevel(Logger::Priority level) {
        m_level = level;
    }
    
    void Logger::SetOutput(std::ostream * out) {
        m_out = out;
    }
}
