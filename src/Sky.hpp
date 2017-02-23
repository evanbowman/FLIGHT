#pragma once

#include <OpenGL/gl3.h>
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Entity.hpp"
#include "FrustumCheck.hpp"
#include "PrimitiveShapes.hpp"

class SkyManager {
    glm::vec3 m_sunPos;
    glm::vec3 m_sunDir;
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
