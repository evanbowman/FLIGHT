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
	
    Optional & operator=(const T & data) {
	*reinterpret_cast<T *>(m_data.data()) = data;
	m_initialized = true;
	return *this;
    }
	
    Optional & operator=(T && data) {
	*reinterpret_cast<T *>(m_data.data()) = std::move(data);
	m_initialized = true;
	return *this;
    }
	
    operator bool() {
	return m_initialized;
    }
	
    T & Value() {
	if (m_initialized) {
	    return *reinterpret_cast<T *>(m_data.data());
	}
	throw NullOptError();
    }
	
    ~Optional() {
	reinterpret_cast<T *>(m_data.data())->~T();
	m_initialized = false;
    }
};
