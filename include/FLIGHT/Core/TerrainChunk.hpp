#pragma once

#include <glm/glm.hpp>
#include <noise/noise.h>
#include <noiseutils.h>

#include <FLIGHT/Core/BB.hpp>
#include <FLIGHT/Entity/Coin.hpp>
#include <FLIGHT/Entity/Entity.hpp>
#include <FLIGHT/Core/MeshBuilder.hpp>
#include <FLIGHT/Core/Random.hpp>
#include <FLIGHT/Graphics/DisplayImpl.hpp>
#include <FLIGHT/Graphics/VBO.hpp>
#include <FLIGHT/Graphics/Vertex.hpp>

namespace FLIGHT {
class TerrainChunk {
public:
    enum class DrawQuality {
        None, // <-- for invisible things
        Despicable,
        Low,
        Medium,
        High
    };
    TerrainChunk(const glm::vec3 & position, utils::NoiseMap & heightMap);
    friend class TerrainManager;
    constexpr static size_t GetVertexCount() {
        return (GetSidelength() + GetMargin()) *
               (GetSidelength() + GetMargin());
    }
    constexpr static size_t GetSidelength() { return 32; }
    constexpr static size_t GetMargin() { return 4; }
    static constexpr const float vertElevationScale = 5.5f;
    static constexpr const float vertSpacing = 1.0f;
    void Display(DisplayImpl & gfx);
    static float GetMaxElevation() { return vertElevationScale * 1.46094f; }
    inline void SetDrawQuality(DrawQuality drawQuality) {
        m_drawQuality = drawQuality;
    }
    inline DrawQuality GetDrawQuality() const {
	return m_drawQuality;
    }
    TerrainChunk(const TerrainChunk &) = delete;
    const TerrainChunk & operator=(const TerrainChunk &) = delete;
    TerrainChunk(TerrainChunk && other);
    TerrainChunk & operator=(TerrainChunk && other);
    ~TerrainChunk();
    const glm::vec3 & GetPosition() const;
    DynamicVBO & GetMeshData();

private:
    glm::vec3 m_position;
    void SpawnCoins(utils::NoiseMap & heightMap);
    void DisplayCoins(DisplayImpl & gfx);
    DrawQuality m_drawQuality;
    DynamicVBO m_meshData;
    std::vector<std::weak_ptr<Coin>> m_coins;
};
}
