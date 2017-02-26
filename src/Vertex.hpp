#pragma once

#include <glm/glm.hpp>

struct VertexPTN {
    glm::vec3 pos;
    glm::vec3 norm;
    glm::vec2 texCoord;
};

struct VertexPT {
    glm::vec3 pos;
    glm::vec2 texCoord;
};

struct VertexPN {
    glm::vec3 pos;
    glm::vec3 norm;
};

struct VertexP {
    glm::vec3 pos;
};
