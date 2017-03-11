#include "LoadBlueprints.hpp"

namespace FLIGHT {
inline static Blueprint ReadPartsList(YAML::Node pl) {
    Blueprint blueprint;
    for (auto it = pl.begin(); it != pl.end(); ++it) {
        Blueprint::Part part;
        if (auto material = (*it)["material"]) {
            part.material = material.as<std::string>();
        }
        if (auto model = (*it)["model"]) {
            part.model = model.as<std::string>();
        } else {
	    throw std::runtime_error("Missing model");
	}
        if (auto texture = (*it)["texture"]) {
            part.texture = texture.as<std::string>();
        }
        if (auto position = (*it)["position"]) {
            if (auto x = position["x"]) {
                part.position.x = x.as<float>();
            }
            if (auto y = position["y"]) {
                part.position.y = y.as<float>();
            }
            if (auto z = position["z"]) {
                part.position.z = z.as<float>();
            }
        }
        if (auto scale = (*it)["scale"]) {
            if (auto x = scale["x"]) {
                part.scale.x = x.as<float>();
            }
            if (auto y = scale["y"]) {
                part.scale.y = y.as<float>();
            }
            if (auto z = scale["z"]) {
                part.scale.z = z.as<float>();
            }
        }
        if (auto rot = (*it)["rotation"]) {
            if (auto x = rot["x"]) {
                part.rotation.x = glm::radians(x.as<float>());
            }
            if (auto y = rot["y"]) {
                part.rotation.y = glm::radians(y.as<float>());
            }
            if (auto z = rot["z"]) {
                part.rotation.z = glm::radians(z.as<float>());
            }
        }
        blueprint.AddPart(part);
    }
    return blueprint;
}

PlaneRegistry LoadPlanes() {
    std::ifstream file(ResourcePath() + "blueprints/planes.yml");
    std::stringstream ss;
    ss << file.rdbuf();
    YAML::Node node = YAML::Load(ss.str());
    PlaneRegistry registry;
    if (auto planes = node["planes"]) {
        for (auto it = planes.begin(); it != planes.end(); ++it) {
            std::string planeName;
            if (auto name = (*it)["name"]) {
                planeName = name.as<std::string>();
            } else {
                throw std::runtime_error(
                    "plane blueprint missing \'name\' tag");
            }
            if (auto partslist = (*it)["parts-list"]) {
                registry[planeName] = ReadPartsList(partslist);
            } else {
                throw std::runtime_error(
                    "plane blueprint missing \'parts-list\' tag");
            }
        }
    }
    return registry;
}
}
