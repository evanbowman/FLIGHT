#pragma once

#include "OpenGL/gl3.h"
#include <noise/noise.h>
#include <noise/noiseutils.h>
#include <glm/glm.hpp>
#include <array>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <algorithm>
#include <utility>
#include <queue>
#include <mutex>
#include "MeshBuilder.hpp"
#include <set>
#include "Plane.hpp"
#include "Error.hpp"
#include "FrustumCheck.hpp"
#include "GuardedResource.hpp"

class Chunk {
public:
    enum class DrawQuality {
	None, // <-- for invisible things
	Despicable,
	Low,
	Medium,
	High
    };
    Chunk();
    friend class TerrainManager;
    constexpr static size_t GetVertexCount() {
	return (GetSidelength() + GetMargin()) * (GetSidelength() + GetMargin());
    }
    constexpr static size_t GetSidelength() {
	return 32;
    }
    constexpr static size_t GetMargin() {
	return 4;
    }
    constexpr static size_t GetIndexCountHQ() {
	return GetIndexCount(1);
    }
    constexpr static size_t GetIndexCountMQ() {
	return GetIndexCount(2);
    }
    constexpr static size_t GetIndexCountLQ() {
	return GetIndexCount(4);
    }
    constexpr static size_t GetIndexCountDQ() {
	return GetIndexCount(8);
    }
    void Display(const glm::mat4 & parentContext,
		 const GLuint shaderProgram);
    inline void SetDrawQuality(DrawQuality drawQuality) {
	m_drawQuality = drawQuality;
    }
private:
    DrawQuality m_drawQuality;
    GLuint m_meshData;
    static GLuint m_indicesHQ;
    static GLuint m_indicesMQ;
    static GLuint m_indicesLQ;
    static GLuint m_indicesDQ;
    static void InitIndexBufs();
    constexpr static size_t GetIndexCount(const size_t scale) {
	return ((((GetSidelength() + GetMargin()) / scale) - 1)
		* (((GetSidelength() + GetMargin()) / scale) - 1)) * 6;
    }
};

class TerrainManager {
    struct UploadReq {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::pair<int, int> index;
    };
    std::map<std::pair<int, int>, utils::NoiseMap> m_heightmapCache;
    LockedResource<std::map<std::pair<int, int>, Chunk>> m_chunks;
    LockedResource<std::vector<std::shared_ptr<UploadReq>>> m_chunkUploadReqs;
    LockedResource<std::set<std::pair<int, int>>> m_chunkCreateReqs;
    LockedResource<std::vector<Chunk>> m_chunkRemovalReqs;
    std::vector<GLuint> m_availableBufs;
    time_t m_seed;
    const utils::NoiseMap & GetHeightMap(const int x, const int y);
    utils::NoiseMap CreateHeightMap(const int x, const int y);
    void PruneHeightMapCache(const std::pair<int, int> & locus);
    void CacheHeightMap(const int x, const int y, utils::NoiseMap && heightmap);
    void CreateChunk(const int x, const int y);
    void RequestChunk(const int x, const int y);
public:
    void SetSeed(const time_t seed);
    time_t GetSeed() const;
    void SwapChunks();
    TerrainManager();
    bool HasWork();
    void UpdateTerrainGen();
    void UpdateChunkLOD(const glm::vec3 & cameraPos, const glm::vec3 & viewDir);
    void Display(const GLuint shaderProgram);
    const TerrainManager & operator=(const TerrainManager &) = delete;
    ~TerrainManager();
};
