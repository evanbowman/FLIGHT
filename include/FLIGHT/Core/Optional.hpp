#pragma once

#include <array>
#include <cstring>
#include <cassert>

class NullOptError : public std::exception {
public:
    const char * what() const noexcept override {
	return "Attempt to access an empty Optional<>";
    }
};

template <typename T>
class Optional {
    std::array<char, sizeof(T)> m_data;
    bool m_initialized;
public:
    Optional() : m_initialized(false) {}

    void Dispose() {
	if (m_initialized) {
	    reinterpret_cast<T *>(m_data.data())->~T();
	    m_initialized = false;
	}
    }
    
    Optional & operator=(const Optional<T> & other) {
	if (other.m_initialized) {
	    m_initialized = other.m_initialized;
	    *reinterpret_cast<T *>(m_data.data()) =
		*reinterpret_cast<T *>(other.m_data.data());
	}
	return *this;
    }

    Optional && operator=(Optional<T> && other) {
	if (other.m_initialized) {
	    m_initialized = other.m_initialized;
	    other.m_initialized = false;
	    *reinterpret_cast<T *>(m_data.data()) =
		std::forward<T>(*reinterpret_cast<T *>(other.m_data.data()));
	}
	return *this;
    }

    Optional(Optional<T> & other) : m_initialized(false) {
	*this = other;
    }

    Optional(Optional<T> && other) : m_initialized(false) {
	*this = std::forward<Optional<T>>(other);
    }
    
    Optional & operator=(const T & data) {
	*reinterpret_cast<T *>(m_data.data()) = data;
	m_initialized = true;
	return *this;
    }
	
    Optional & operator=(T && data) {
	*reinterpret_cast<T *>(m_data.data()) = std::forward<T>(data);
	m_initialized = true;
	return *this;
    }
	
    operator bool() const {
	return m_initialized;
    }
	
    T & Value() {
	if (m_initialized) {
	    return *reinterpret_cast<T *>(m_data.data());
	}
	throw NullOptError();
    }

    ~Optional() {
	Dispose();
    }
};
