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
#include "Shader.hpp"
#include "Vertex.hpp"
#include "TerrainChunk.hpp"

namespace FLIGHT {    
    class TerrainManager {
	struct UploadReq {
	    std::vector<glm::vec3> vertices;
	    std::vector<glm::vec3> normals;
	    std::pair<int, int> index;
	};
	std::map<std::pair<int, int>, utils::NoiseMap> m_heightmapCache;
	LockedResource<std::map<std::pair<int, int>, std::shared_ptr<TerrainChunk>>> m_chunks;
	LockedResource<std::vector<std::shared_ptr<UploadReq>>> m_chunkUploadReqs;
	LockedResource<std::set<std::pair<int, int>>> m_chunkCreateReqs;
	LockedResource<std::vector<std::shared_ptr<TerrainChunk>>> m_chunkRemovalReqs;
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
