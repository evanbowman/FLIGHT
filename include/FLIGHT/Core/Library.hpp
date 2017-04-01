#pragma once
#if defined(FLIGHT_MAC) or defined(FLIGHT_LINUX)
#include <dlfcn.h>
#endif
#include <stdexcept>

class Library {
    void * m_handle;
public:
    explicit Library(const std::string & path) {
#if defined(FLIGHT_MAC) or defined(FLIGHT_LINUX)
	m_handle = dlopen(path.c_str(), RTLD_LAZY);
	if (!m_handle) {
	    throw std::runtime_error("Failed to load library from path: " + path);
	}
#endif
    }
	
    void * GetFunction(const std::string name) {
#if defined(FLIGHT_MAC) or defined(FLIGHT_LINUX)
	void * function = dlsym(m_handle, name.c_str());
	char * error;
	if ((error = dlerror()) != nullptr) {
	    throw std::runtime_error("Library::GetFunction failed with error: " + std::string(error));
	}
	return function;
#endif		
    }
	
    ~Library() {
#if defined(FLIGHT_MAC) or defined(FLIGHT_LINUX)
	dlclose(m_handle);
#endif
    }
};
