#pragma once

#include <FLIGHT/Entity/Entity.hpp>
#include "FrustumCheck.hpp"
#include <FLIGHT/Graphics/PrimitiveShapes.hpp>
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace FLIGHT {
class SkyManager {
    glm::vec3 m_sunPos;
    glm::vec3 m_skydomeLocus;
    glm::vec2 m_rot;
    bool m_sunVisible;

public:
    struct Flare {
        float displFactor;
        float scale;
        glm::vec3 position;
        float intensity;
    };
    void Update(const glm::vec3 & cameraPos, const glm::vec3 & viewDir);
    void Display(DisplayImpl & renderer);
    bool SunVisible() const {
	return m_sunVisible;
    }
    const glm::vec3 & GetSunPos() const {
	return m_sunPos;
    }
    const glm::vec3 & GetSkydomeCenter() const {
	return m_skydomeLocus;
    }
    const glm::vec2 & GetSkydomeRot() const {
	return m_rot;
    }
};
}
