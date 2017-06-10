#pragma once

#include "Plotter.hpp"

namespace FLIGHT {
class CoinPlotter : public Plotter {
public:
    void Invoke(utils::NoiseMap & map, TerrainChunk & chunk) override;
};
}
