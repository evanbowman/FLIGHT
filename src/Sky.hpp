#pragma once

#ifdef FLIGHT_MAC
#include <OpenGL/gl3.h>
#elif FLIGHT_WINDOWS
#include <GL/glew.h>
#endif
#include "Entity.hpp"
#include "FrustumCheck.hpp"
#include "PrimitiveShapes.hpp"
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
    void Display();
    void DoLensFlare();
};
}
