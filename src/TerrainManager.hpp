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
#include "MeshBuilder.hpp"

class Chunk {
    struct MeshInfo {
	GLuint data;
	GLuint indicesHQ;
	GLuint indexCountHQ;
	GLuint indicesMQ;
	GLuint indexCountMQ;
	GLuint indicesLQ;
	GLuint indexCountLQ;
	GLuint indicesDQ;
	GLuint indexCountDQ;
    };
    MeshInfo m_meshInfo;
public:
    enum class DrawQuality {
	Despicable,
	Low,
	Medium,
	High
    };
    Chunk();
    friend class TerrainManager;
    constexpr static size_t GetSidelength() {
	return 64;
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
    ~Chunk();
};

class TerrainManager {
    std::map<std::pair<int, int>, Chunk> m_chunks;
    struct NoiseGenerator {
	module::RidgedMulti module;
	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane builder;
    };
    NoiseGenerator m_noiseGen;
    void CreateChunk(const int x, const int y);
public:
    TerrainManager();
    void Display(const glm::vec3 & cameraPos, const GLuint shaderProgram);
};
