#pragma once

#include "config.h"

#include <cstring>
#include <string>
#ifdef FLIGHT_WINDOWS
#include <Windows.h>
#elif FLIGHT_LINUX
#include <linux/limits.h>
#include <unistd.h>
#endif

std::string ResourcePath();
