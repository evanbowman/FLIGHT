#pragma once

#include <cstring>
#include <glm/glm.hpp>

#include "GameMath.hpp"

namespace FLIGHT {
bool IntersectsFrustum(const glm::vec3 & objectPos, const glm::vec3 & eyePos,
                       const glm::vec3 & viewDir, const float margin = 0.f);
}
