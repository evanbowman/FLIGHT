#include <FLIGHT/Core/Game.hpp>
#include <FLIGHT/Core/TerrainChunk.hpp>

namespace FLIGHT {
void TerrainChunk::SpawnCoins(utils::NoiseMap & heightMap) {
    RANDOM::Seed(Singleton<Game>::Instance().GetSeed() ^
                 ((unsigned)m_position.x ^ (unsigned)m_position.y));
    int x = RANDOM::Get() % 32;
    int z = RANDOM::Get() % 32;
    const float heightVal = heightMap.GetValue(x, z) * vertElevationScale;
    if (heightVal < -3.5f) {
        glm::vec3 createPos{m_position.x + x * vertSpacing, heightVal + 5.5f,
                            m_position.z + z * vertSpacing};
        m_coins.push_back(Singleton<Game>::Instance().CreateSolid<Coin>(createPos));
    }
}

TerrainChunk::TerrainChunk(const glm::vec3 & position,
                           utils::NoiseMap & heightMap)
    : m_drawQuality(TerrainChunk::DrawQuality::None) {
    m_position = position;
    SpawnCoins(heightMap);
}

const glm::vec3 & TerrainChunk::GetPosition() const {
    return m_position;
}

DynamicVBO & TerrainChunk::GetMeshData() {
    return m_meshData;
}
    
void TerrainChunk::DisplayCoins(DisplayImpl & renderer) {
    if (m_coins.size() > 0) {
        for (auto it = m_coins.begin(); it != m_coins.end();) {
            if (auto coin = (*it).lock()) {
		coin->Display(renderer);
                ++it;
            } else {
                it = m_coins.erase(it);
            }
        }
    }
}

TerrainChunk::~TerrainChunk() {
    for (auto & coin : m_coins) {
        if (auto coinSp = coin.lock()) {
            coinSp->SetDeallocFlag();
        }
    }
}

void TerrainChunk::Display(DisplayImpl & renderer) {
    if (m_drawQuality == DrawQuality::None) {
        return;
    }
    DisplayCoins(renderer);
    renderer.Dispatch(*this);
}

TerrainChunk & TerrainChunk::operator=(TerrainChunk && other) {
    m_coins = std::move(other.m_coins);
    other.m_coins.clear();
    return *this;
}

TerrainChunk::TerrainChunk(TerrainChunk && other) { *this = std::move(other); }
}
