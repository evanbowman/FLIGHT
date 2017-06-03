#pragma once

#include <unordered_map>

#include "Blueprint.hpp"

namespace FLIGHT {
using PlaneRegistry = std::unordered_map<std::string, Blueprint>;

PlaneRegistry LoadPlanes();
}
