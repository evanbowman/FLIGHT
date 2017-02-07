#pragma once

#include "OpenGL/gl3.h"
#include <noise/noise.h>
#include <noise/noiseutils.h>
#include <glm/glm.hpp>
#include <array>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <utility>
#include <queue>
#include "MeshBuilder.hpp"

class Chunk {
    struct MeshInfo {
	GLuint mesh;
	GLuint indicesHQ;
	GLuint indicesMQ;
	GLuint indicesLQ;
	GLuint indicesDQ;
    };
    MeshInfo m_meshInfo;
    constexpr static size_t GetIndexCount(const size_t scale) {
	return ((((GetSidelength() + GetMargin()) / scale) - 1)
		* (((GetSidelength() + GetMargin()) / scale) - 1)) * 6;
    }
public:
    enum class DrawQuality {
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
	return 64;
    }
    constexpr static size_t GetMargin() {
	return 8;
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
    Chunk(const Chunk & other) = delete;
    Chunk(Chunk && other) {
	m_meshInfo = other.m_meshInfo;
    }
    Chunk & operator=(Chunk && other) {
	m_meshInfo = other.m_meshInfo;
	return *this;
    }
    void Display(const glm::mat4 & parentContext,
		 const GLuint shaderProgram,
		 const DrawQuality quality);
    // Destructor intentionally left out. I don't want
    // chunks to be copyable, but their vertex buffers
    // are managed by the Terrain manager so there's nothing
    // more to do here...
};

class TerrainManager {
    struct CreateReq {
	Mesh meshHQ;
	Mesh meshMQ;
	Mesh meshLQ;
	Mesh meshDQ;
	std::vector<glm::vec3> normals;
	std::pair<int, int> index;
    };
    std::map<std::pair<int, int>, Chunk> m_chunks;
    std::vector<CreateReq> m_chunkCreateRequests;
    std::vector<Chunk> m_chunkDeleteRequests;
    struct AvailableBuffers {
	std::vector<GLuint> mesh;
	std::vector<GLuint> indexHQ;
	std::vector<GLuint> indexMQ;
	std::vector<GLuint> indexLQ;
	std::vector<GLuint> indexDQ;
    };
    AvailableBuffers m_availableBufs;
    struct NoiseGenerator {
	module::RidgedMulti module;
	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane builder;
    };
    Chunk CreateChunk();
    NoiseGenerator m_noiseGen;
    void RequestChunk(const int x, const int y);
public:
    void SwapChunks();
    TerrainManager();
    void Display(const glm::vec3 & cameraPos, const glm::vec3 & viewDir,
		 const GLuint shaderProgram);
};
