#pragma once

#include <string>
#include <vector>

namespace FLIGHT {
struct ManifestData {
    std::vector<std::string> textures;
    std::vector<std::string> models;
    std::vector<std::string> materials;
};

ManifestData LoadManifest();
}
