#pragma once

#include <chrono>
#include <thread>

template <unsigned cap>
class UpdateCap {
    std::chrono::high_resolution_clock::time_point created;
public:
    UpdateCap() : created(std::chrono::high_resolution_clock::now()) {}
    UpdateCap(const UpdateCap &) = delete;
    ~UpdateCap() {
	using namespace std::chrono;
	const auto done = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(done - created);
	static const microseconds usecCap{cap};
	std::this_thread::sleep_for(usecCap - duration);
    }
};
