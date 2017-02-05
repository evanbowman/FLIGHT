#pragma once

#include <noise/noise.h>
#include <noise/noiseutils.h>
#include <random>
#include <OpenGL/gl3.h>
#include <array>
#include <glm/glm.hpp>
#include "Vertex.hpp"
#include "MapChunk.hpp"

class MapSector {
    std::array<float, 4> m_bounds;
};
