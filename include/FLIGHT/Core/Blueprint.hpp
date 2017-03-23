#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace FLIGHT {
class Blueprint {
public:
    struct Part {
        std::string material;
        std::string model;
        std::string texture;
        glm::vec3 position;
        glm::vec3 scale{1.f, 1.f, 1.f};
        glm::vec3 rotation;
    };
    const std::vector<Part> & GetParts() const;
    void AddPart(const Part & part);

private:
    std::vector<Part> m_parts;
};
}
