#pragma once

template <typename T> class Singleton {
public:
    static T & Instance() {
        static T m_instance;
        return m_instance;
    }
};
