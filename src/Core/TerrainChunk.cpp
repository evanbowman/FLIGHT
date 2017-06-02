#include <FLIGHT/Core/Game.hpp>
#include <FLIGHT/Core/TerrainChunk.hpp>

namespace FLIGHT {
void TerrainChunk::OnCreate(utils::NoiseMap & heightMap) {
    auto & game = Singleton<Game>::Instance();
    for (auto & plotter : game.GetTerrainMgr().GetPlotters()) {
        plotter->Invoke(heightMap, *this);
    }
}

TerrainChunk::TerrainChunk(const glm::vec3 & position,
                           utils::NoiseMap & heightMap)
    : m_drawQuality(TerrainChunk::DrawQuality::None) {
    m_position = position;
    OnCreate(heightMap);
}

void TerrainChunk::AppendChild(std::weak_ptr<Entity> child) {
    m_children.push_back(child);
}

const glm::vec3 & TerrainChunk::GetPosition() const { return m_position; }

DynamicVBO & TerrainChunk::GetMeshData() { return m_meshData; }

void TerrainChunk::DisplayChildren(DisplayImpl & renderer) {
    if (m_children.size() > 0) {
        for (auto it = m_children.begin(); it not_eq m_children.end();) {
            if (auto childSp = (*it).lock()) {
                childSp->Display(renderer);
                ++it;
            } else {
                it = m_children.erase(it);
            }
        }
    }
}

TerrainChunk::~TerrainChunk() {
    for (auto & child : m_children) {
        if (auto childSp = child.lock()) {
            childSp->SetDeallocFlag();
        }
    }
}

void TerrainChunk::Display(DisplayImpl & renderer) {
    if (m_drawQuality == DrawQuality::None) {
        return;
    }
    DisplayChildren(renderer);
    renderer.Dispatch(*this);
}

TerrainChunk & TerrainChunk::operator=(TerrainChunk && other) {
    m_children = std::move(other.m_children);
    return *this;
}

TerrainChunk::TerrainChunk(TerrainChunk && other) { *this = std::move(other); }
}
