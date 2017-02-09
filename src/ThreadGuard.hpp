#pragma once

#include <thread>

class ThreadGuard {
    std::thread thread;
public:
    template<typename Function, typename ...Args>
    explicit ThreadGuard(Function && f, Args && ...args) :
	thread(std::forward<Function>(f), std::forward<Args>(args)...) {}
    ThreadGuard(std::thread && _thread) : thread(std::move(_thread)) {}
    ThreadGuard(const ThreadGuard &) = delete;
    ThreadGuard(ThreadGuard && other) { thread = std::move(other.thread); }
    ThreadGuard & operator=(ThreadGuard &) = delete;
    ThreadGuard & operator=(ThreadGuard && other) {
	thread = std::move(other.thread);
	return *this;
    }
    const std::thread & get() const { return thread; }
    std::thread && release() {
	return std::move(thread);
    }
    ~ThreadGuard() { if (thread.joinable()) thread.join(); }
};
