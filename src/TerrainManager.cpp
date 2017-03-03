#include "TerrainManager.hpp"

struct TerrainVert {
    glm::vec3 pos;
    glm::vec3 norm;
};

GLuint TerrainChunk::m_indicesHQ;
GLuint TerrainChunk::m_indicesMQ;
GLuint TerrainChunk::m_indicesLQ;
GLuint TerrainChunk::m_indicesDQ;

void TerrainChunk::InitIndexBufs() {
    constexpr const size_t chunkSize = GetSidelength() + GetMargin();
    MeshBuilder meshBuilderHQ(chunkSize, chunkSize);
    MeshBuilder meshBuilderMQ(chunkSize, chunkSize);
    MeshBuilder meshBuilderLQ(chunkSize, chunkSize);
    MeshBuilder meshBuilderDQ(chunkSize, chunkSize);
    int vertIndex = 0;
    for (size_t y = 0; y < chunkSize; ++y) {
        for (size_t x = 0; x < chunkSize; ++x) {
            if (x < chunkSize - 1 && y < chunkSize - 1) {
                meshBuilderHQ.AddTriangle(vertIndex, vertIndex + chunkSize + 1,
                                          vertIndex + chunkSize);
                meshBuilderHQ.AddTriangle(vertIndex + chunkSize + 1, vertIndex,
                                          vertIndex + 1);
                if (x % 2 == 0 && y % 2 == 0 && x < chunkSize - 2 &&
                    y < chunkSize - 2) {
                    meshBuilderMQ.AddTriangle(vertIndex,
                                              vertIndex + chunkSize * 2 + 2,
                                              vertIndex + chunkSize * 2);
                    meshBuilderMQ.AddTriangle(vertIndex + chunkSize * 2 + 2,
                                              vertIndex, vertIndex + 2);
                }
                if (x % 3 == 0 && y % 3 == 0 && x < chunkSize - 3 &&
                    y < chunkSize - 3) {
                    meshBuilderLQ.AddTriangle(vertIndex,
                                              vertIndex + chunkSize * 3 + 3,
                                              vertIndex + chunkSize * 3);
                    meshBuilderLQ.AddTriangle(vertIndex + chunkSize * 3 + 3,
                                              vertIndex, vertIndex + 3);
                }
                if (x % 4 == 0 && y % 4 == 0 && x < chunkSize - 4 &&
                    y < chunkSize - 4) {
                    meshBuilderDQ.AddTriangle(vertIndex,
                                              vertIndex + chunkSize * 4 + 4,
                                              vertIndex + chunkSize * 4);
                    meshBuilderDQ.AddTriangle(vertIndex + chunkSize * 4 + 4,
                                              vertIndex, vertIndex + 4);
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 TerrainChunk::GetIndexCountHQ() * sizeof(GLushort),
                 meshHQ.triangles.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &m_indicesMQ);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesMQ);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 TerrainChunk::GetIndexCountMQ() * sizeof(GLushort),
                 meshMQ.triangles.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &m_indicesLQ);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesLQ);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 TerrainChunk::GetIndexCountLQ() * sizeof(GLushort),
                 meshLQ.triangles.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &m_indicesDQ);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesDQ);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 TerrainChunk::GetIndexCountDQ() * sizeof(GLushort),
                 meshDQ.triangles.data(), GL_STATIC_DRAW);
}

TerrainChunk::TerrainChunk()
    : m_drawQuality(TerrainChunk::DrawQuality::None), m_meshData{} {}

void TerrainChunk::Display(const glm::mat4 & parentContext,
                           ShaderProgram & shader) {
    if (m_drawQuality == DrawQuality::None) {
        return;
    }
    glm::mat4 invTransModel = glm::transpose(glm::inverse(parentContext));
    shader.SetUniformMat4("invTransModel", invTransModel);
    shader.SetUniformMat4("model", parentContext);
    glBindBuffer(GL_ARRAY_BUFFER, m_meshData);
    shader.SetVertexAttribPtr("position", 3, GL_FLOAT, sizeof(TerrainVert));
    shader.SetVertexAttribPtr("normal", 3, GL_FLOAT, sizeof(TerrainVert),
                              sizeof(glm::vec3));
    switch (m_drawQuality) {
    case DrawQuality::High:
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesHQ);
        glDrawElements(GL_TRIANGLES, TerrainChunk::GetIndexCountHQ(),
                       GL_UNSIGNED_SHORT, 0);
        break;

    case DrawQuality::Medium:
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesMQ);
        glDrawElements(GL_TRIANGLES, TerrainChunk::GetIndexCountMQ(),
                       GL_UNSIGNED_SHORT, 0);
        break;

    case DrawQuality::Low:
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesLQ);
        glDrawElements(GL_TRIANGLES, TerrainChunk::GetIndexCountLQ(),
                       GL_UNSIGNED_SHORT, 0);
        break;

    case DrawQuality::Despicable:
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesDQ);
        glDrawElements(GL_TRIANGLES, TerrainChunk::GetIndexCountDQ(),
                       GL_UNSIGNED_SHORT, 0);
        break;

    case DrawQuality::None:
        break;
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

TerrainManager::TerrainManager() : m_hasWork(false) {
    // Online multiplayer idea, host simply shares seed?
    TerrainChunk::InitIndexBufs();
    m_seed = 0;
    RequestChunk(0, 0);
}

void TerrainManager::SetSeed(const time_t seed) { m_seed = seed; }

time_t TerrainManager::GetSeed() const { return m_seed; }

void TerrainManager::UpdateChunkLOD(const glm::vec3 & cameraPos,
                                    const glm::vec3 & viewDir) {
    auto chunksLkRef = m_chunks.Lock();
    auto & chunks = chunksLkRef.first.get();
    for (auto it = chunks.begin(); it != chunks.end();) {
        const auto chunkSize = TerrainChunk::GetSidelength();
        float displ = TerrainChunk::vertSpacing * chunkSize;
        const int x = it->first.first;
        const int y = it->first.second;
        glm::vec3 modelPos{x * displ, 0, y * displ};
        glm::mat4 model;
        model = glm::translate(model, modelPos);
        float absDist = std::abs(
            glm::distance(cameraPos, {modelPos.x, modelPos.y, modelPos.z}));
        if (IntersectsFrustum(modelPos, cameraPos, viewDir, 40.f)) {
            if (absDist < 280) {
                it->second.SetDrawQuality(TerrainChunk::DrawQuality::High);
            } else if (absDist < 330) {
                it->second.SetDrawQuality(TerrainChunk::DrawQuality::Medium);
            } else if (absDist < 360) {
                it->second.SetDrawQuality(TerrainChunk::DrawQuality::Low);
            } else {
                it->second.SetDrawQuality(
                    TerrainChunk::DrawQuality::Despicable);
            }
            for (int i = x - 1; i < x + 2; ++i) {
                for (int j = y - 1; j < y + 2; ++j) {
                    modelPos = {i * displ - displ / 2, 0,
                                j * displ - displ / 2};
                    if (std::abs(glm::distance(cameraPos, modelPos)) < 400.f) {
                        if (chunks.find({i, j}) == chunks.end()) {
                            RequestChunk(i, j);
                        }
                    }
                }
            }
        } else {
            it->second.SetDrawQuality(TerrainChunk::DrawQuality::None);
        }
        if (absDist < 400) {
            ++it;
        } else {
            auto remQueueLkRef = m_chunkRemovalReqs.Lock();
            remQueueLkRef.first.get().push_back(it->second);
            it = chunks.erase(it);
        }
    }
}

void TerrainManager::Display(ShaderProgram & shader) {
    auto chunksLkRef = m_chunks.Lock();
    auto & chunks = chunksLkRef.first.get();
    for (auto & chunkMapNode : chunks) {
        const auto chunkSize = TerrainChunk::GetSidelength();
        float displ = TerrainChunk::vertSpacing * chunkSize;
        const int x = chunkMapNode.first.first;
        const int y = chunkMapNode.first.second;
        glm::vec3 modelPos{x * displ - displ / 2, 0, y * displ - displ / 2};
        glm::mat4 model;
        model = glm::translate(model, modelPos);
        chunkMapNode.second.Display(model, shader);
    }
}

utils::NoiseMap TerrainManager::CreateHeightMap(const int x, const int y) {
    module::RidgedMulti module;
    utils::NoiseMap heightMap;
    utils::NoiseMapBuilderPlane builder;
    module.SetSeed(m_seed);
    module.SetFrequency(0.5f);
    builder.SetSourceModule(module);
    builder.SetDestNoiseMap(heightMap);
    builder.SetDestSize(TerrainChunk::GetSidelength(),
                        TerrainChunk::GetSidelength());
    builder.SetBounds(x * 2, x * 2 + 2, y * 2, y * 2 + 2);
    builder.Build();
    return heightMap;
}

void TerrainManager::PruneHeightMapCache(const std::pair<int, int> & locus) {
    glm::vec2 locusPos(locus.first, locus.second);
    for (auto hmNode = m_heightmapCache.begin();
         hmNode != m_heightmapCache.end();) {
        glm::vec2 nodePos(hmNode->first.first, hmNode->first.second);
        if (std::abs(glm::length(locusPos - nodePos)) > 10) {
            hmNode = m_heightmapCache.erase(hmNode);
        } else {
            ++hmNode;
        }
    }
}

void TerrainManager::CacheHeightMap(const int x, const int y,
                                    utils::NoiseMap && heightMap) {
    m_heightmapCache[{x, y}] = heightMap;
    if (m_heightmapCache.size() > 900) {
        PruneHeightMapCache({x, y});
    }
}

const utils::NoiseMap & TerrainManager::GetHeightMap(const int x, const int y) {
    auto node = m_heightmapCache.find({x, y});
    if (node == m_heightmapCache.end()) {
        CacheHeightMap(x, y, CreateHeightMap(x, y));
        return m_heightmapCache[{x, y}];
    } else {
        return node->second;
    }
}

void TerrainManager::CreateChunk(const int x, const int y) {
    auto & heightMap = GetHeightMap(x, y);
    // To prevent seams from appearing between the chunks, also compute the
    // heightmaps
    // for the regions to the right and below, and add redundant geometry
    // accordingly.
    // This tripples the cost of generating elevation data, but honestly thats a
    // small
    // price to be paid for seamless procedurally generated terrain at runtime.
    auto & heightMapEast = GetHeightMap(x + 1, y);
    auto & heightMapSouth = GetHeightMap(x, y + 1);
    auto & heightMapSouthEast = GetHeightMap(x + 1, y + 1);
    static constexpr const size_t margin = TerrainChunk::GetMargin();
    static constexpr const size_t chunkSize =
        TerrainChunk::GetSidelength() + margin;
    MeshBuilder meshBuilder(chunkSize, chunkSize);
    int vertIndex = 0;
    std::array<std::array<glm::vec3, chunkSize>, chunkSize> vertices;
    for (size_t y = 0; y < chunkSize; ++y) {
        for (size_t x = 0; x < chunkSize; ++x) {
            glm::vec3 vert;
            static const size_t realChunkSize = chunkSize - margin;
            if (x < realChunkSize && y < realChunkSize) {
                vert = {x * TerrainChunk::vertSpacing,
                        heightMap.GetValue(x, y) *
                            TerrainChunk::vertElevationScale,
                        y * TerrainChunk::vertSpacing};
            } else if (x < realChunkSize && y >= realChunkSize) {
                vert = {x * TerrainChunk::vertSpacing,
                        heightMapSouth.GetValue(x, y - realChunkSize) *
                            TerrainChunk::vertElevationScale * 0.96f,
                        y * TerrainChunk::vertSpacing};
            } else if (y < realChunkSize && x >= realChunkSize) {
                vert = {x * TerrainChunk::vertSpacing,
                        heightMapEast.GetValue(x - realChunkSize, y) *
                            TerrainChunk::vertElevationScale * 0.96f,
                        y * TerrainChunk::vertSpacing};
            } else {
                vert = {x * TerrainChunk::vertSpacing,
                        heightMapSouthEast.GetValue(x - realChunkSize,
                                                    y - realChunkSize) *
                            TerrainChunk::vertElevationScale * 0.96f,
                        y * TerrainChunk::vertSpacing};
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
        std::copy(normals[i].begin(), normals[i].end(),
                  std::back_inserter(linearNorms));
    }
    for (auto & element : linearNorms) {
        element = glm::normalize(element);
    }
    auto uploadQueueLkRef = m_chunkUploadReqs.Lock();
    uploadQueueLkRef.first.get().push_back(std::shared_ptr<UploadReq>(
        new UploadReq{meshBuilder.GetMesh().vertices, linearNorms, {x, y}}));
}

void TerrainManager::UpdateTerrainGen() {
    std::set<std::pair<int, int>> createReqs;
    {
        auto createQueueLkRef = m_chunkCreateReqs.Lock();
        auto & reqs = createQueueLkRef.first.get();
        for (auto & req : reqs) {
            createReqs.insert(req);
        }
    }
    for (auto & req : createReqs) {
        this->CreateChunk(req.first, req.second);
    }
    {
        auto createQueueLkRef = m_chunkCreateReqs.Lock();
        for (auto & req : createReqs) {
            createQueueLkRef.first.get().erase(req);
        }
        if (createQueueLkRef.first.get().empty()) {
            m_hasWork = false;
        }
    }
}

void TerrainManager::RequestChunk(const int x, const int y) {
    auto uploadQueueLkRef = m_chunkUploadReqs.Lock();
    bool existsInUploadQueue = false;
    for (auto & element : uploadQueueLkRef.first.get()) {
        if (element->index == std::pair<int, int>(x, y)) {
            existsInUploadQueue = true;
        }
    }
    if (!existsInUploadQueue) {
        auto createQueueLkRef = m_chunkCreateReqs.Lock();
        createQueueLkRef.first.get().insert({x, y});
        m_hasWork = true;
    }
}

bool TerrainManager::HasWork() { return m_hasWork; }

void TerrainManager::SwapChunks() {
    {
        auto removeQueueLkRef = m_chunkRemovalReqs.Lock();
        for (auto & chunk : removeQueueLkRef.first.get()) {
            m_availableBufs.push_back(chunk.m_meshData);
        }
        removeQueueLkRef.first.get().clear();
    }
    std::vector<std::shared_ptr<UploadReq>> uploadReqs;
    {
        auto uploadQueueLkRef = m_chunkUploadReqs.Lock();
        auto & upreqs = uploadQueueLkRef.first.get();
        std::copy(upreqs.begin(), upreqs.end(), std::back_inserter(uploadReqs));
    }
    for (auto & req : uploadReqs) {
        TerrainChunk chunk;
        if (!m_availableBufs.empty()) {
            chunk.m_meshData = m_availableBufs.back();
            m_availableBufs.pop_back();
        } else {
            glGenBuffers(1, &chunk.m_meshData);
            glBindBuffer(GL_ARRAY_BUFFER, chunk.m_meshData);
            glBufferData(GL_ARRAY_BUFFER,
                         TerrainChunk::GetVertexCount() * sizeof(TerrainVert),
                         nullptr, GL_DYNAMIC_DRAW);
        }
        glBindBuffer(GL_ARRAY_BUFFER, chunk.m_meshData);
        std::vector<TerrainVert> data;
        for (size_t i = 0; i < req->vertices.size(); ++i) {
            data.push_back({req->vertices[i], req->normals[i]});
        }
        glBufferSubData(GL_ARRAY_BUFFER, 0,
                        TerrainChunk::GetVertexCount() * sizeof(TerrainVert),
                        data.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        {
            auto chunkLkRef = m_chunks.Lock();
            chunkLkRef.first.get()[{req->index.first, req->index.second}] =
                chunk;
        }
    }
    {
        auto uploadQueueLkRef = m_chunkUploadReqs.Lock();
        auto & uploadQueue = uploadQueueLkRef.first.get();
        for (auto & req : uploadReqs) {
            for (auto it = uploadQueue.begin(); it != uploadQueue.end();) {
                if ((*it)->index == req->index) {
                    it = uploadQueue.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }
    AssertGLStatus("chunk recycling");
}

TerrainManager::~TerrainManager() {
    auto chunksLkRef = m_chunks.Lock();
    for (auto & mapNode : chunksLkRef.first.get()) {
        glDeleteBuffers(1, &mapNode.second.m_meshData);
    }
    for (GLuint buf : m_availableBufs) {
        glDeleteBuffers(1, &buf);
    }
}
