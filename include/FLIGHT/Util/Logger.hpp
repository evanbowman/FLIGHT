#pragma once

#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <mutex>

namespace FLIGHT {
class Logger {
public:
    enum class Priority {
        Debug,
        Info,
        Warn,
        Critical
    };
    void Log(Priority level, const std::string & message);
    void SetOutput(std::ostream * out);
    void SetLevel(Priority level);
    
private:
    Priority m_level = Priority::Info;
    std::ostream * m_out = &std::cout;
    std::mutex m_mutex;
};
}
