#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include <pugixml.hpp>
#include <FLIGHT/Util/ResourcePath.hpp>

namespace FLIGHT {
struct ManifestData {
    std::vector<std::string> textures;
    std::vector<std::string> models;
    std::vector<std::string> materials;
};

ManifestData LoadManifest();
}
