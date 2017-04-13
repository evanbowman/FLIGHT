#pragma once

#include <fstream>
#include <glm/glm.hpp>
#include <sstream>
#include <unordered_map>
#include <pugixml.hpp>

#include "Blueprint.hpp"
#include "ResourcePath.hpp"

namespace FLIGHT {
using PlaneRegistry = std::unordered_map<std::string, Blueprint>;

PlaneRegistry LoadPlanes();
}
