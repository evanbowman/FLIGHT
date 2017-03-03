#pragma once

#include <mutex>
#include <utility>
#include <functional>

// Use this to encapsulate data shared across multiple threads, to guarantee
// unique ownership. By calling Lock(), a thread borrows a reference to the
// resource, and is the only one that can modify it.
namespace FLIGHT {    
    template <typename T>
    class LockedResource {
	T m_resource;
	std::mutex m_lock;
    public:
	std::pair<std::reference_wrapper<T>, std::unique_lock<std::mutex>> Lock() {
	    return {m_resource, std::unique_lock<std::mutex>(m_lock)};
	}
    };
}
