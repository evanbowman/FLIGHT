#pragma once

#include <fstream>
#include <sstream>
#include <unordered_map>
#include <yaml-cpp/yaml.h>

#include "Blueprint.hpp"
#include "ResourcePath.hpp"

namespace FLIGHT {
using PlaneRegistry = std::unordered_map<std::string, Blueprint>;

PlaneRegistry LoadPlanes();
}
