#pragma once

#include <cstring>
#include <string>
#if defined(_WIN32) or defined(_WIN64)
#include <Windows.h>
#elif __LINUX__
#include <linux/limits.h>
#include <unistd.h>
#endif

std::string ResourcePath();
