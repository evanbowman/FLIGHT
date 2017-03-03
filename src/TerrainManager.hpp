#pragma once

#ifdef FLIGHT_MAC
#include "OpenGL/gl3.h"
#elif FLIGHT_WINDOWS
#include <GL/glew.h>
#endif
#include <noise/noise.h>
#include <noiseutils.h>
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

#include "Error.hpp"
#include "FrustumCheck.hpp"
#include "GuardedResource.hpp"
#include "BB.hpp"
#include "Entity.hpp"
#include "Shader.hpp"

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
	TerrainChunk();
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
	    return GetIndexCount(3);
	}
	constexpr static size_t GetIndexCountDQ() {
	    return GetIndexCount(4);
	}
	static constexpr const float vertElevationScale = 5.5f;
	static constexpr const float vertSpacing = 1.0f;
	void Display(const glm::mat4 & parentContext,
		     ShaderProgram & shader);
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
	LockedResource<std::map<std::pair<int, int>, TerrainChunk>> m_chunks;
	LockedResource<std::vector<std::shared_ptr<UploadReq>>> m_chunkUploadReqs;
	LockedResource<std::set<std::pair<int, int>>> m_chunkCreateReqs;
	LockedResource<std::vector<TerrainChunk>> m_chunkRemovalReqs;
	std::vector<GLuint> m_availableBufs;
	time_t m_seed;
	const utils::NoiseMap & GetHeightMap(const int x, const int y);
	utils::NoiseMap CreateHeightMap(const int x, const int y);
	void PruneHeightMapCache(const std::pair<int, int> & locus);
	void CacheHeightMap(const int x, const int y, utils::NoiseMap && heightmap);
	void CreateChunk(const int x, const int y);
	void RequestChunk(const int x, const int y);
	bool m_hasWork;
    public:
	void SetSeed(const time_t seed);
	time_t GetSeed() const;
	void SwapChunks();
	TerrainManager();
	bool HasWork();
	void UpdateTerrainGen();
	void UpdateChunkLOD(const glm::vec3 & cameraPos, const glm::vec3 & viewDir);
	void Display(ShaderProgram & shader);
	const TerrainManager & operator=(const TerrainManager &) = delete;
	~TerrainManager();
    };
}
