#pragma once

#include "Plotter.hpp"

namespace FLIGHT {
    class EnemySpawnPlotter : public Plotter {
    public:
        void Invoke(utils::NoiseMap & map, TerrainChunk & chunk) override;
    };
}
