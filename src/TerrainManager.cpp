#include "TerrainManager.hpp"

struct TerrainVert {
    glm::vec3 pos;
    glm::vec3 norm;
};

GLuint Chunk::m_indicesHQ;
GLuint Chunk::m_indicesMQ;
GLuint Chunk::m_indicesLQ;
GLuint Chunk::m_indicesDQ;

namespace Profiling {
    size_t vboCount = 0;
}

void Chunk::InitIndexBufs() {
    constexpr const size_t chunkSize = GetSidelength() + GetMargin();
    MeshBuilder meshBuilderHQ(chunkSize, chunkSize);
    MeshBuilder meshBuilderMQ(chunkSize, chunkSize);
    MeshBuilder meshBuilderLQ(chunkSize, chunkSize);
    MeshBuilder meshBuilderDQ(chunkSize, chunkSize);
    int vertIndex = 0;
    for (size_t y = 0; y < chunkSize; ++y) {
	for (size_t x = 0; x < chunkSize; ++x) {
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
		    meshBuilderDQ.AddTriangle(vertIndex,
					      vertIndex + chunkSize * 8 + 8,
					      vertIndex + chunkSize * 8);
		    meshBuilderDQ.AddTriangle(vertIndex + chunkSize * 8 + 8,
					      vertIndex, vertIndex + 8);
		}
	    }
	    ++vertIndex;
	}
    }
    auto meshHQ = meshBuilderHQ.GetMesh();
    auto meshMQ = meshBuilderMQ.GetMesh();
    auto meshLQ = meshBuilderLQ.GetMesh();
    auto meshDQ = meshBuilderDQ.GetMesh();
    glGenBuffers(1, &m_indicesHQ);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesHQ);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Chunk::GetIndexCountHQ() * sizeof(GLushort),
		 meshHQ.triangles.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &m_indicesMQ);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesMQ);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Chunk::GetIndexCountMQ() * sizeof(GLushort),
		 meshMQ.triangles.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &m_indicesLQ);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesLQ);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Chunk::GetIndexCountLQ() * sizeof(GLushort),
		 meshLQ.triangles.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &m_indicesDQ);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesDQ);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Chunk::GetIndexCountDQ() * sizeof(GLushort),
		 meshDQ.triangles.data(), GL_STATIC_DRAW);
}

Chunk::Chunk() : m_drawQuality(Chunk::DrawQuality::None), m_meshData{} {}

static const float vertSpacing = 0.5f;

void Chunk::Display(const glm::mat4 & parentContext,
		    const GLuint shaderProgram) {
    if (m_drawQuality == DrawQuality::None) {
	return;
    }
    const GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    const GLint invTransModelLoc = glGetUniformLocation(shaderProgram, "invTransModel");
    const GLint posAttribLoc = glGetAttribLocation(shaderProgram, "position");
    const GLint normAttribLoc = glGetAttribLocation(shaderProgram, "normal");
    glm::mat4 invTransModel = glm::transpose(glm::inverse(parentContext));
    glUniformMatrix4fv(invTransModelLoc, 1, GL_FALSE, glm::value_ptr(invTransModel));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(parentContext));
    glBindBuffer(GL_ARRAY_BUFFER, m_meshData);
    glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVert), 0);
    glVertexAttribPointer(normAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVert),
			  (void *)sizeof(glm::vec3));
    switch (m_drawQuality) {
    case DrawQuality::High:
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesHQ);
	glDrawElements(GL_TRIANGLES, Chunk::GetIndexCountHQ(), GL_UNSIGNED_SHORT, 0);
    	break;

    case DrawQuality::Medium:
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesMQ);
	glDrawElements(GL_TRIANGLES, Chunk::GetIndexCountMQ(), GL_UNSIGNED_SHORT, 0);
	break;

    case DrawQuality::Low:
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesLQ);
	glDrawElements(GL_TRIANGLES, Chunk::GetIndexCountLQ(), GL_UNSIGNED_SHORT, 0);
	break;

    case DrawQuality::Despicable:
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesDQ);
	glDrawElements(GL_TRIANGLES, Chunk::GetIndexCountDQ(), GL_UNSIGNED_SHORT, 0);
	break;

    case DrawQuality::None:
	break;
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

TerrainManager::TerrainManager() {
    // Online multiplayer idea, host simply shares seed?
    Chunk::InitIndexBufs();
    m_noiseGen.module.SetSeed(time(nullptr));
    m_noiseGen.module.SetFrequency(0.5f);
    m_noiseGen.builder.SetSourceModule(m_noiseGen.module);
    m_noiseGen.builder.SetDestNoiseMap(m_noiseGen.heightMap);
    m_noiseGen.builder.SetDestSize(Chunk::GetSidelength(), Chunk::GetSidelength());
    RequestChunk(0, 0);
}

#include <iostream>

bool ChunkIsInFrontOfView(const glm::vec3 & chunkPos, const glm::vec3 & cameraPos, const glm::vec3 & viewDir) {
    // This one may require some explanation... so the function approximates the
    // frustum by building two planes angled at 45 degrees in opposite directions,
    // and checks whether stuff is in front of them with a simple dot product.
    glm::mat3 rotMatrix;
    static const float rot = 45.f;
    const float leftRot[] = {
	std::cos(glm::radians(-rot)), 0, std::sin(glm::radians(-rot)),
	0, 1, 0,
	-std::sin(glm::radians(-rot)), 0, std::cos(glm::radians(-rot))
    };
    const float rightRot[] = {
	std::cos(glm::radians(rot)), 0, std::sin(glm::radians(rot)),
	0, 1, 0,
	-std::sin(glm::radians(rot)), 0, std::cos(glm::radians(rot))
    };
    std::memcpy(glm::value_ptr(rotMatrix), leftRot, sizeof(leftRot));
    const glm::vec3 leftPlaneDir = rotMatrix * viewDir;
    std::memcpy(glm::value_ptr(rotMatrix), rightRot, sizeof(rightRot));
    const glm::vec3 rightPlaneDir = rotMatrix * viewDir;
    auto leftPlaneToChunk = chunkPos - cameraPos;
    auto rightPlaneToChunk = chunkPos - cameraPos;
    double leftDot = glm::dot(leftPlaneToChunk, leftPlaneDir);
    double rightDot = glm::dot(rightPlaneToChunk, rightPlaneDir);
    return leftDot < 40.f && rightDot < 40.f;
}

// Rather than doing costly occlusion calculations, it can be observed that when flying
// low the level of detail of far off elements is less pronounced.
static float VisibilityHeuristic(const float height) {
    auto maxElevation = Plane::GetElevationLimit();
    return (maxElevation / height) * 7;
}

void TerrainManager::UpdateChunkLOD(const glm::vec3 & cameraPos, const glm::vec3 & viewDir) {
    for (auto it = m_chunks.begin(); it != m_chunks.end();) {
	const auto chunkSize = Chunk::GetSidelength();
	float displ = vertSpacing * chunkSize;
	const int x = it->first.first;
	const int y = it->first.second;
	glm::vec3 modelPos{x * displ, 0, y * displ};
	glm::mat4 model;
	    model = glm::translate(model, modelPos);
	    float absDist = std::abs(glm::distance(cameraPos, {
			modelPos.x, modelPos.y, modelPos.z}));
	if (ChunkIsInFrontOfView(modelPos, cameraPos, viewDir)) {
	    if (absDist < 60 && cameraPos.y < 15.f) {
		it->second.SetDrawQuality(Chunk::DrawQuality::High);
	    } else if (absDist < 270 - VisibilityHeuristic(cameraPos.y)) {
		it->second.SetDrawQuality(Chunk::DrawQuality::Medium);
	    } else if (absDist < 330 - VisibilityHeuristic(cameraPos.y)) {
	        it->second.SetDrawQuality(Chunk::DrawQuality::Low);
	    } else {
		it->second.SetDrawQuality(Chunk::DrawQuality::Despicable);
	    }
	    for (int i = x - 1; i < x + 2; ++i) {
		for (int j = y - 1; j < y + 2; ++j) {
		    modelPos = {i * displ - displ / 2, 0, j * displ - displ / 2};
		    if (std::abs(glm::distance(cameraPos, modelPos)) < 400.f) {
			if (m_chunks.find({i, j}) == m_chunks.end()) {
			    RequestChunk(i, j);
			}
		    }
		}
	    }
	} else {
	    it->second.SetDrawQuality(Chunk::DrawQuality::None);
	}
	if (absDist < 400) {
	    ++it;
	} else {
	    std::lock_guard<std::mutex> lk(m_removeQueueMtx);
	    m_chunkRemovalReqs.push_back(it->second);
	    it = m_chunks.erase(it);
	}
    }
}

void TerrainManager::Display(const GLuint shaderProgram) {
    for (auto & chunkMapNode : m_chunks) {
	const auto chunkSize = Chunk::GetSidelength();
	float displ = vertSpacing * chunkSize;
	const int x = chunkMapNode.first.first;
	const int y = chunkMapNode.first.second;
	glm::vec3 modelPos{x * displ - displ / 2, 0, y * displ - displ / 2};
	glm::mat4 model;
	model = glm::translate(model, modelPos);
	chunkMapNode.second.Display(model, shaderProgram);
    }
}

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
    static constexpr const size_t margin = Chunk::GetMargin();
    static constexpr const size_t chunkSize = Chunk::GetSidelength() + margin;
    MeshBuilder meshBuilder(chunkSize, chunkSize);
    int vertIndex = 0;
    static const float vertElevationScale = 5.5f;
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
	    meshBuilder.AddVertex(vertIndex, vert);
	    vertices[y][x] = vert;
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
    std::lock_guard<std::mutex> lk(m_uploadQueueMtx);
    m_chunkUploadReqs.push_back(std::shared_ptr<UploadReq>(new UploadReq {
		meshBuilder.GetMesh().vertices,
		    linearNorms,
		    {x, y}
	    }));
}

void TerrainManager::UpdateTerrainGen() {
    std::set<std::pair<int, int>> createReqs;
    {
	std::lock_guard<std::mutex> lk(m_createQueueMtx);
	for (auto & req : m_chunkCreateReqs) {
	    createReqs.insert(req);
	}
    }
    for (auto & req : createReqs) {
	this->CreateChunk(req.first, req.second);
    }
    {
	std::lock_guard<std::mutex> lk(m_createQueueMtx);
	for (auto & req : createReqs) {
	    m_chunkCreateReqs.erase(req);
	}
    }
}

void TerrainManager::RequestChunk(const int x, const int y) {
    std::lock_guard<std::mutex> lk(m_createQueueMtx);
    std::lock_guard<std::mutex> lk2(m_uploadQueueMtx);
    // When Chunk vertex data is created but it hasn't yet been
    // uploaded to video memory, there is no longer an instance in the
    // create queue. Obviously it would be dumb to recreate the same
    // data set, so also make sure the data isn't already in the
    // upload queue before allowing another create request to be
    // processed.
    bool existsInUploadQueue = false;
    for (auto & element : m_chunkUploadReqs) {
	if (element->index == std::pair<int, int>(x,y)) {
	    existsInUploadQueue = true;
	}
    }
    if (!existsInUploadQueue) {
	m_chunkCreateReqs.insert({x, y});
    }
}

bool TerrainManager::IsLoadingChunks() {
    return m_chunkUploadReqs.size() > 0 || m_chunkCreateReqs.size() > 0;
}

void TerrainManager::SwapChunks() {
    {
	std::lock_guard<std::mutex> lk(m_removeQueueMtx);
	for (auto & chunk : m_chunkRemovalReqs) {
	    m_availableBufs.push_back(chunk.m_meshData);
	}
	m_chunkRemovalReqs.clear();
    }
    std::vector<std::shared_ptr<UploadReq>> uploadReqs;
    {
	std::lock_guard<std::mutex> lk(m_uploadQueueMtx);
	std::copy(m_chunkUploadReqs.begin(), m_chunkUploadReqs.end(), std::back_inserter(uploadReqs));
    }
    for (auto & req : uploadReqs) {
	Chunk chunk;
	if (!m_availableBufs.empty()) {
	    chunk.m_meshData = m_availableBufs.back();
	    m_availableBufs.pop_back();
	} else {
	    glGenBuffers(1, &chunk.m_meshData);
	    glBindBuffer(GL_ARRAY_BUFFER, chunk.m_meshData);
	    glBufferData(GL_ARRAY_BUFFER, Chunk::GetVertexCount() * sizeof(TerrainVert),
			 nullptr, GL_DYNAMIC_DRAW);
	}
	glBindBuffer(GL_ARRAY_BUFFER, chunk.m_meshData);
	std::vector<TerrainVert> data;
	for (size_t i = 0; i < req->vertices.size(); ++i) {
	    data.push_back({req->vertices[i], req->normals[i]});
	}
	glBufferSubData(GL_ARRAY_BUFFER, 0, Chunk::GetVertexCount() * sizeof(TerrainVert),
			data.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	m_chunks[{req->index.first, req->index.second}] = std::move(chunk);
    }
    {
	std::lock_guard<std::mutex> lk(m_uploadQueueMtx);
        for (auto & req : uploadReqs) {
	    for (auto it = m_chunkUploadReqs.begin(); it != m_chunkUploadReqs.end();) {
		if ((*it)->index == req->index) {
		    it = m_chunkUploadReqs.erase(it);
		} else {
		    ++it;
		}
	    }
	}
    }
}
