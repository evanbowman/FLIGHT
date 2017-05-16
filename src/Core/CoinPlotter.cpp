#include <FLIGHT/Core/CoinPlotter.hpp>
#include <FLIGHT/Core/Game.hpp>

namespace FLIGHT {
void CoinPlotter::Invoke(utils::NoiseMap & map, TerrainChunk & chunk) {
    const auto & position = chunk.GetPosition();
    RANDOM::Seed(Singleton<Game>::Instance().GetSeed() ^
                 ((unsigned)position.x ^ (unsigned)position.y));
    int x = RANDOM::Get() % 32;
    int z = RANDOM::Get() % 32;
    const float heightVal =
        map.GetValue(x, z) * TerrainChunk::vertElevationScale;
    if (heightVal < -3.5f) {
        glm::vec3 createPos{position.x + x * TerrainChunk::vertSpacing,
                            heightVal + 5.5f,
                            position.z + z * TerrainChunk::vertSpacing};
        chunk.AppendChild(
            Singleton<Game>::Instance().CreateSolid<Coin>(createPos));
    }
}
}
