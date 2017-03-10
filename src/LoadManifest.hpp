#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include <yaml-cpp/yaml.h>

#include "ResourcePath.hpp"

namespace FLIGHT {
struct ManifestData {
    std::vector<std::string> textures;
    std::vector<std::string> models;
};

ManifestData LoadManifest();
}
