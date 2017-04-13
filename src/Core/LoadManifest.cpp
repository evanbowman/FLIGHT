#include <FLIGHT/Core/LoadManifest.hpp>

namespace FLIGHT {
ManifestData LoadManifest() {
    std::ifstream file(ResourcePath() + "manifest.yml");
    std::stringstream ss;
    ss << file.rdbuf();
    YAML::Node node = YAML::Load(ss.str());
    ManifestData manifest;
    if (auto textures = node["textures"]) {
        for (auto it = textures.begin(); it not_eq textures.end(); ++it) {
            manifest.textures.push_back(it->as<std::string>());
        }
    }
    if (auto models = node["models"]) {
        for (auto it = models.begin(); it not_eq models.end(); ++it) {
            manifest.models.push_back(it->as<std::string>());
        }
    }
    if (auto materials = node["materials"]) {
        for (auto it = materials.begin(); it not_eq materials.end(); ++it) {
            manifest.materials.push_back(it->as<std::string>());
        }
    }
    return manifest;
}
}
