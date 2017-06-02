#pragma once

#include "MeshBuilder.hpp"
#include <algorithm>
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <map>
#include <mutex>
#include <noise/noise.h>
#include <noise/noiseutils.h>
#include <queue>
#include <set>
#include <utility>
#include <FLIGHT/Graphics/DisplayImpl.hpp>

#include "Plotter.hpp"
#include <FLIGHT/Graphics/Error.hpp>
#include "FrustumCheck.hpp"
#include "GuardedResource.hpp"
#include <FLIGHT/Graphics/Shader.hpp>
#include "TerrainChunk.hpp"
#include <FLIGHT/Util/Time.hpp>
#include <FLIGHT/Graphics/Vertex.hpp>

namespace FLIGHT {
class TerrainManager {
    struct UploadReq {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::pair<int, int> index;
    };
    std::vector<std::unique_ptr<Plotter>> m_plotters;
    std::map<std::pair<int, int>, utils::NoiseMap> m_heightmapCache;
    LockedResource<std::map<std::pair<int, int>, std::shared_ptr<TerrainChunk>>>
        m_chunks;
    LockedResource<std::vector<std::shared_ptr<UploadReq>>> m_chunkUploadReqs;
    LockedResource<std::set<std::pair<int, int>>> m_chunkCreateReqs;
    LockedResource<std::vector<std::shared_ptr<TerrainChunk>>>
        m_chunkRemovalReqs;
    std::vector<DynamicVBO> m_availableBufs;
    const utils::NoiseMap & GetHeightMap(const int x, const int y);
    void PruneHeightMapCache();
    void CacheHeightMap(const int x, const int y, utils::NoiseMap && heightmap);
    void CreateChunk(const int x, const int y);
    bool m_hasWork;

protected:
    virtual utils::NoiseMap CreateHeightMap(const int x, const int y) = 0;

public:
    std::vector<std::unique_ptr<Plotter>> & GetPlotters();
    void ClearPlotters();
    void AppendPlotter(std::unique_ptr<Plotter> plotter);
    void RequestChunk(const int x, const int y);
    void Reset();
    void SwapChunks();
    TerrainManager();
    bool HasWork();
    bool LoadingComplete();
    utils::NoiseMap * GetHeightMap(const std::pair<int, int> & coord);
    void UpdateTerrainGen();
    void UpdateChunkLOD(const glm::vec3 & cameraPos, const glm::vec3 & viewDir);
    void Display(DisplayImpl & renderer);
    const TerrainManager & operator=(const TerrainManager &) = delete;
    virtual ~TerrainManager() {}
};

class MountainousTerrain : public TerrainManager {
protected:
    utils::NoiseMap CreateHeightMap(const int x, const int y) override;
};

class DesertTerrain : public TerrainManager {
protected:
    utils::NoiseMap CreateHeightMap(const int x, const int y) override;
};
}
