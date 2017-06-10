#include <FLIGHT/Core/LoadManifest.hpp>
#include <FLIGHT/Util/ResourcePath.hpp>
#include <fstream>
#include <pugixml.hpp>
#include <sstream>

namespace FLIGHT {
ManifestData LoadManifest() {
    pugi::xml_document doc;
    auto res = doc.load_file((ResourcePath() + "manifest.xml").c_str());
    if (not res) {
        throw std::runtime_error("Failed to import manifest file: " +
                                 std::string(res.description()));
    }
    ManifestData manifest;
    auto root = *doc.begin();
    auto textures = root.child("Textures");
    auto models = root.child("Models");
    auto materials = root.child("Materials");
    auto regFileData = [](std::vector<std::string> & target,
                          pugi::xml_node & fileInfo) {
        auto name = fileInfo.attribute("name");
        if (name) {
            target.push_back(name.value());
        }
    };
    if (textures) {
        for (auto & file : textures) {
            regFileData(manifest.textures, file);
        }
    } else {
        throw std::runtime_error("Manifest missing Textures element");
    }
    if (models) {
        for (auto & file : models) {
            regFileData(manifest.models, file);
        }
    } else {
        throw std::runtime_error("Manifest missing Models element");
    }
    if (materials) {
        for (auto & file : materials) {
            regFileData(manifest.materials, file);
        }
    } else {
        throw std::runtime_error("Manifest missing Materials element");
    }
    return manifest;
}
}
