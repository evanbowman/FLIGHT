#pragma once

#include <FLIGHT/Core/BB.hpp>
#include <FLIGHT/Core/ChunkSize.hpp>
#include <FLIGHT/Entity/Entity.hpp>
#include <FLIGHT/Graphics/DisplayImpl.hpp>
#include <FLIGHT/Graphics/VBO.hpp>
#include <FLIGHT/Graphics/Vertex.hpp>
#include <glm/glm.hpp>
#include <list>
#include <noise/noise.h>
#include <noise/noiseutils.h>

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
        return (CHUNK_SIZE + GetMargin()) * (CHUNK_SIZE + GetMargin());
    }
    constexpr static size_t GetMargin() { return 4; }
    static constexpr const float vertElevationScale = 5.5f;
    static constexpr const float vertSpacing = 1.0f;
    void Display(DisplayImpl & renderer);
    static float GetMaxElevation() { return vertElevationScale * 1.46094f; }
    inline void SetDrawQuality(DrawQuality drawQuality) {
        m_drawQuality = drawQuality;
    }
    inline DrawQuality GetDrawQuality() const { return m_drawQuality; }
    TerrainChunk(const TerrainChunk &) = delete;
    const TerrainChunk & operator=(const TerrainChunk &) = delete;
    TerrainChunk(TerrainChunk && other);
    TerrainChunk & operator=(TerrainChunk && other);
    ~TerrainChunk();
    const glm::vec3 & GetPosition() const;
    DynamicVBO & GetMeshData();
    void AppendChild(std::weak_ptr<Entity> child);

private:
    glm::vec3 m_position;
    void OnCreate(utils::NoiseMap & heightMap);
    void DisplayChildren(DisplayImpl & renderer);
    DrawQuality m_drawQuality;
    DynamicVBO m_meshData;
    std::list<std::weak_ptr<Entity>> m_children;
};
}
