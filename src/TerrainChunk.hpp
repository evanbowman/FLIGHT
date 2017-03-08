#pragma once

#include <glm/glm.hpp>
#include <noise/noise.h>
#include <noiseutils.h>

#include "BB.hpp"
#include "Entity.hpp"
#include "MeshBuilder.hpp"
#include "Vertex.hpp"
#include "Coin.hpp"
#include "Random.hpp"

namespace FLIGHT {
    class TerrainChunk : public Solid {
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
	void Display(ShaderProgram & shader) override;
	void Update(const Time) override {
	    m_inbox.Clear();
	}
	AABB GetAABB() override;
        float GetMaxElevation() const {
	    return vertElevationScale * 1.46094f;
	}
	inline void SetDrawQuality(DrawQuality drawQuality) {
	    m_drawQuality = drawQuality;
	}
	TerrainChunk(const TerrainChunk &) = delete;
	const TerrainChunk & operator=(const TerrainChunk &) = delete;
	TerrainChunk(TerrainChunk && other);
	TerrainChunk & operator=(TerrainChunk && other);
	~TerrainChunk();
    private:
	void SpawnCoins(utils::NoiseMap & heightMap);
	void DisplayCoins();
	DrawQuality m_drawQuality;
	GLuint m_meshData;
	std::vector<std::weak_ptr<Coin>> m_coins;
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
}
