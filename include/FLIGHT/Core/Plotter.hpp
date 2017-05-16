#pragma once

#include <noise/noiseutils.h>

namespace FLIGHT {
    class TerrainChunk;
    
    class Plotter {
    public:
        virtual ~Plotter() {}
        virtual void Invoke(utils::NoiseMap & map, TerrainChunk & chunk) = 0;
    };
}
