#include "TerrainManager.hpp"

struct TerrainVert {
    glm::vec3 pos;
    glm::vec3 norm;
};

Chunk::Chunk() : m_meshInfo{} {}

static const float vertSpacing = 0.5f;

Chunk::~Chunk() {
    // FIXME
}

void Chunk::Display(const glm::mat4 & parentContext,
		    const GLuint shaderProgram, const DrawQuality quality) {
    const GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    const GLint invTransModelLoc = glGetUniformLocation(shaderProgram, "invTransModel");
    const GLint posAttribLoc = glGetAttribLocation(shaderProgram, "position");
    const GLint normAttribLoc = glGetAttribLocation(shaderProgram, "normal");
    glm::mat4 invTransModel = glm::transpose(glm::inverse(parentContext));
    glUniformMatrix4fv(invTransModelLoc, 1, GL_FALSE, glm::value_ptr(invTransModel));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(parentContext));
    glBindBuffer(GL_ARRAY_BUFFER, m_meshInfo.data);
    glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVert), 0);
    glVertexAttribPointer(normAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVert),
			  (void *)sizeof(glm::vec3));
    switch (quality) {
    case DrawQuality::High:
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshInfo.indicesHQ);
	glDrawElements(GL_TRIANGLES, m_meshInfo.indexCountHQ, GL_UNSIGNED_SHORT, 0);
    	break;

    case DrawQuality::Medium:
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshInfo.indicesMQ);
	glDrawElements(GL_TRIANGLES, m_meshInfo.indexCountMQ, GL_UNSIGNED_SHORT, 0);
	break;

    case DrawQuality::Low:
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshInfo.indicesLQ);
	glDrawElements(GL_TRIANGLES, m_meshInfo.indexCountLQ, GL_UNSIGNED_SHORT, 0);
	break;

    case DrawQuality::Despicable:
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshInfo.indicesDQ);
	glDrawElements(GL_TRIANGLES, m_meshInfo.indexCountDQ, GL_UNSIGNED_SHORT, 0);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

TerrainManager::TerrainManager() {
    // Online multiplayer idea, host simply shares seed?
    m_noiseGen.module.SetSeed(time(nullptr));
    m_noiseGen.module.SetFrequency(0.5f);
    m_noiseGen.builder.SetSourceModule(m_noiseGen.module);
    m_noiseGen.builder.SetDestNoiseMap(m_noiseGen.heightMap);
    m_noiseGen.builder.SetDestSize(Chunk::GetSidelength(), Chunk::GetSidelength());
    CreateChunk(0, 0);
    CreateChunk(0, 1);
    CreateChunk(0, -1);
    CreateChunk(1, 0);
    CreateChunk(1, 1);
    CreateChunk(1, -1);
    CreateChunk(-1, 0);
    CreateChunk(-1, 1);
    CreateChunk(-1, -1);
    CreateChunk(2, 0);
    CreateChunk(2, 1);
    CreateChunk(2, -1);
    CreateChunk(-2, 0);
    CreateChunk(-2, 1);
    CreateChunk(-2, -1);
}

bool ChunkIntersectsFrustum(const Chunk & chunk) {
    return true;
}

void TerrainManager::Display(const glm::vec3 & cameraPos, const GLuint shaderProgram) {
    for (auto & chunkNode : m_chunks) {
	if (ChunkIntersectsFrustum(chunkNode.second)) {
	    glm::mat4 model;
	    const auto chunkSize = Chunk::GetSidelength();
	    float displ = vertSpacing * chunkSize;
	    glm::vec3 modelPos{chunkNode.first.first * displ, 0, chunkNode.first.second * displ};
	    model = glm::translate(model, modelPos);
	    float absDist = std::abs(glm::distance(cameraPos, {
			modelPos.x + chunkSize / 2, modelPos.y, modelPos.z + chunkSize / 2}));
	    if (absDist < 80) {
		chunkNode.second.Display(model, shaderProgram, Chunk::DrawQuality::High);
	    } else if (absDist < 100) {
		chunkNode.second.Display(model, shaderProgram, Chunk::DrawQuality::Medium);
	    } else if (absDist < 140) {
		chunkNode.second.Display(model, shaderProgram, Chunk::DrawQuality::Low);
	    } else {
		chunkNode.second.Display(model, shaderProgram, Chunk::DrawQuality::Despicable);
	    }
	}
    }
}

// FIXME: border regions between chunks
void TerrainManager::CreateChunk(const int x, const int y) {
    m_noiseGen.builder.SetBounds(x * 2, x * 2 + 2, y * 2, y * 2 + 2);
    m_noiseGen.builder.SetDestNoiseMap(m_noiseGen.heightMap);
    m_noiseGen.builder.Build();
    // To prevent seams from appearing between the chunks, also compute the heightmaps
    // for the regions to the right and below, and add redundant geometry accordingly.
    // This tripples the cost of generating elevation data, but honestly thats a small
    // price to be paid for seamless procedurally generated terrain at runtime. Perhaps I
    // could cache the heightmaps in CPU memory...
    utils::NoiseMap heightMapEast, heightMapSouth, heightMapSouthEast;
    m_noiseGen.builder.SetBounds(x * 2 + 2, x * 2 + 4, y * 2, y * 2 + 2);
    m_noiseGen.builder.SetDestNoiseMap(heightMapEast);
    m_noiseGen.builder.Build();
    m_noiseGen.builder.SetBounds(x * 2, x * 2 + 2, y * 2 + 2, y * 2 + 4);
    m_noiseGen.builder.SetDestNoiseMap(heightMapSouth);
    m_noiseGen.builder.Build();
    m_noiseGen.builder.SetBounds(x * 2 + 2, x * 2 + 4, y * 2 + 2, y * 2 + 4);
    m_noiseGen.builder.SetDestNoiseMap(heightMapSouthEast);
    m_noiseGen.builder.Build();
    static constexpr const size_t margin = 8;
    static constexpr const size_t chunkSize = Chunk::GetSidelength() + margin;
    MeshBuilder meshBuilderHQ(chunkSize, chunkSize);
    MeshBuilder meshBuilderMQ(chunkSize, chunkSize);
    MeshBuilder meshBuilderLQ(chunkSize, chunkSize);
    MeshBuilder meshBuilderDQ(chunkSize, chunkSize);
    int vertIndex = 0;
    static const float vertElevationScale = 5.f;
    std::array<std::array<glm::vec3, chunkSize>, chunkSize> vertices;
    for (size_t y = 0; y < chunkSize; ++y) {
	for (size_t x = 0; x < chunkSize; ++x) {
	    glm::vec3 vert;
	    static const size_t realChunkSize = chunkSize - margin;
	    if (x < realChunkSize && y < realChunkSize) {
	        vert = {x * vertSpacing,
			m_noiseGen.heightMap.GetValue(x, y) * vertElevationScale,
			y * vertSpacing};
	    } else if (x < realChunkSize && y >= realChunkSize) {
	        vert = {x * vertSpacing,
			heightMapSouth.GetValue(x, y - realChunkSize)
			* vertElevationScale * 0.96f,
			y * vertSpacing};
	    } else if (y < realChunkSize && x >= realChunkSize) {
	        vert = {x * vertSpacing,
			heightMapEast.GetValue(x - realChunkSize, y)
			* vertElevationScale * 0.96f,
			y * vertSpacing};
	    } else {
	        vert = {x * vertSpacing,
			heightMapSouthEast.GetValue(x - realChunkSize, y - realChunkSize)
			* vertElevationScale * 0.96f,
			y * vertSpacing};
	    }
	    meshBuilderHQ.AddVertex(vertIndex, vert);
	    vertices[y][x] = vert;
	    if (x < chunkSize - 1 && y < chunkSize - 1) {
		meshBuilderHQ.AddTriangle(vertIndex,
					  vertIndex + chunkSize + 1,
					  vertIndex + chunkSize);
		meshBuilderHQ.AddTriangle(vertIndex + chunkSize + 1,
					  vertIndex,
					  vertIndex + 1);
		if (x % 2 == 0 && y % 2 == 0 && x < chunkSize - 2 && y < chunkSize - 2) {
		    meshBuilderMQ.AddTriangle(vertIndex,
					      vertIndex + chunkSize * 2 + 2,
					      vertIndex + chunkSize * 2);
		    meshBuilderMQ.AddTriangle(vertIndex + chunkSize * 2 + 2,
					      vertIndex,
					      vertIndex + 2);
		}
		if (x % 4 == 0 && y % 4 == 0 && x < chunkSize - 4 && y < chunkSize - 4) {
		    meshBuilderLQ.AddTriangle(vertIndex,
					      vertIndex + chunkSize * 4 + 4,
					      vertIndex + chunkSize * 4);
		    meshBuilderLQ.AddTriangle(vertIndex + chunkSize * 4 + 4,
					      vertIndex, vertIndex + 4);
		}
		if (x % 8 == 0 && y % 8 == 0 && x < chunkSize - 8 && y < chunkSize - 8) {
		    meshBuilderLQ.AddTriangle(vertIndex,
					      vertIndex + chunkSize * 8 + 8,
					      vertIndex + chunkSize * 8);
		    meshBuilderLQ.AddTriangle(vertIndex + chunkSize * 8 + 8,
					      vertIndex, vertIndex + 8);
		}
	    }
	    ++vertIndex;
	}
    }
    std::array<std::array<glm::vec3, chunkSize>, chunkSize> normals{};
    for (size_t y = 0; y < chunkSize; ++y) {
	for (size_t x = 0; x < chunkSize; ++x) {
	    if (x < chunkSize - 1 && y < chunkSize - 1) {
		glm::vec3 p0 = vertices[y][x];
		glm::vec3 p1 = vertices[y + 1][x];
		glm::vec3 p2 = vertices[y][x + 1];
		glm::vec3 e1 = p1 - p0;
		glm::vec3 e2 = p2 - p0;
		glm::vec3 normal = glm::cross(e1, e2);
		normal = glm::normalize(normal);
		normals[y][x] += normal;
		normals[y + 1][x] += normal;
		normals[y][x + 1] += normal;
		
		p0 = vertices[y + 1][x + 1];
		e1 = p1 - p0;
		e2 = p2 - p0;
		normal = glm::cross(e2, e1);
		normal = glm::normalize(normal);
		normals[y + 1][x + 1] += normal;
		normals[y + 1][x] += normal;
		normals[y][x + 1] += normal;
	    }
	}
    }
    std::vector<glm::vec3> linearNorms;
    for (size_t i = 0; i < chunkSize; ++i) {
	std::copy(normals[i].begin(), normals[i].end(), std::back_inserter(linearNorms));
    }
    for (auto & element : linearNorms) {
	element = glm::normalize(element);
    }
    auto meshHQ = meshBuilderHQ.GetMesh();
    auto meshMQ = meshBuilderMQ.GetMesh();
    auto meshLQ = meshBuilderLQ.GetMesh();
    auto meshDQ = meshBuilderDQ.GetMesh();
    Chunk chunk;
    glGenBuffers(1, &chunk.m_meshInfo.indicesHQ);
    glGenBuffers(1, &chunk.m_meshInfo.indicesMQ);
    glGenBuffers(1, &chunk.m_meshInfo.indicesLQ);
    glGenBuffers(1, &chunk.m_meshInfo.indicesDQ);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk.m_meshInfo.indicesHQ);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshHQ.triangles.size() * sizeof(GLushort),
		 meshHQ.triangles.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk.m_meshInfo.indicesMQ);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshMQ.triangles.size() * sizeof(GLushort),
		 meshMQ.triangles.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk.m_meshInfo.indicesLQ);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshLQ.triangles.size() * sizeof(GLushort),
		 meshLQ.triangles.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk.m_meshInfo.indicesDQ);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshDQ.triangles.size() * sizeof(GLushort),
		 meshDQ.triangles.data(), GL_STATIC_DRAW);
    chunk.m_meshInfo.indexCountHQ = meshHQ.triangles.size();
    chunk.m_meshInfo.indexCountMQ = meshMQ.triangles.size();
    chunk.m_meshInfo.indexCountLQ = meshLQ.triangles.size();
    chunk.m_meshInfo.indexCountDQ = meshDQ.triangles.size();
    glGenBuffers(1, &chunk.m_meshInfo.data);
    glBindBuffer(GL_ARRAY_BUFFER, chunk.m_meshInfo.data);
    std::vector<TerrainVert> data;
    for (size_t i = 0; i < meshHQ.vertices.size(); ++i) {
	data.push_back({meshHQ.vertices[i], linearNorms[i]});
    }
    const auto terrainDataSize = data.size() * sizeof(TerrainVert);
    glBufferData(GL_ARRAY_BUFFER, terrainDataSize, data.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    m_chunks[{x, y}] = std::move(chunk);
}
