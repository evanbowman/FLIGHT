#pragma once

#include <string>
#include <iostream>
#include <chrono>

namespace FLIGHT {
    class Logger {
        std::chrono::high_resolution_clock::time_point m_start =
            std::chrono::high_resolution_clock::now();
        std::ostream * m_target = &std::cout;
        
    public:
        void SetTarget(std::ostream & target);
        void Write(const std::string & message);
    };
}
