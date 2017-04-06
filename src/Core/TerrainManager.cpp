#include <FLIGHT/Core/Game.hpp>
#include <FLIGHT/Core/TerrainManager.hpp>

namespace FLIGHT {
TerrainManager::TerrainManager() : m_hasWork(false) {}

utils::NoiseMap *
TerrainManager::GetHeightMap(const std::pair<int, int> & coord) {
    auto location = m_heightmapCache.find(coord);
    if (location != m_heightmapCache.end()) {
        return &location->second;
    }
    return nullptr;
}

void TerrainManager::UpdateChunkLOD(const glm::vec3 & cameraPos,
                                    const glm::vec3 & viewDir) {
    auto chunksLkRef = m_chunks.Lock();
    auto & chunks = chunksLkRef.first.get();
    for (auto it = chunks.begin(); it != chunks.end();) {
        const auto chunkSize = TerrainChunk::GetSidelength();
        float displ = TerrainChunk::vertSpacing * chunkSize;
        const int x = it->first.first;
        const int y = it->first.second;
        glm::vec3 modelPos{x * displ + displ / 2, 0, y * displ + displ / 2};
        glm::mat4 model;
        model = glm::translate(model, modelPos);
        float absDist = std::abs(
            glm::distance(cameraPos, {modelPos.x, modelPos.y, modelPos.z}));
        if (IntersectsFrustum(modelPos, cameraPos, viewDir, 40.f)) {
            if (absDist < 280) {
                it->second->SetDrawQuality(TerrainChunk::DrawQuality::High);
            } else if (absDist < 330) {
                it->second->SetDrawQuality(TerrainChunk::DrawQuality::Medium);
            } else if (absDist < 360) {
                it->second->SetDrawQuality(TerrainChunk::DrawQuality::Low);
            } else {
                it->second->SetDrawQuality(
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
            it->second->SetDrawQuality(TerrainChunk::DrawQuality::None);
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

void TerrainManager::Display(DisplayImpl & renderer) {
    auto chunksLkRef = m_chunks.Lock();
    auto & chunks = chunksLkRef.first.get();
    for (auto & chunkMapNode : chunks) {
        chunkMapNode.second->Display(renderer);
    }
}

utils::NoiseMap MountainousTerrain::CreateHeightMap(const int x, const int y) {
    module::RidgedMulti module;
    utils::NoiseMap heightMap;
    utils::NoiseMapBuilderPlane builder;
    module.SetSeed(Singleton<Game>::Instance().GetSeed());
    module.SetFrequency(0.5f);
    builder.SetSourceModule(module);
    builder.SetDestNoiseMap(heightMap);
    builder.SetDestSize(TerrainChunk::GetSidelength(),
                        TerrainChunk::GetSidelength());
    builder.SetBounds(x * 2, x * 2 + 2, y * 2, y * 2 + 2);
    builder.Build();
    return heightMap;
}

utils::NoiseMap DesertTerrain::CreateHeightMap(const int x, const int y) {
    throw std::runtime_error("Desert terrain incomplete...");
}

void TerrainManager::PruneHeightMapCache() {
    float displ = TerrainChunk::vertSpacing * TerrainChunk::GetSidelength();
    auto cameraPos = Singleton<Game>::Instance().GetCamera().GetPosition();
    glm::vec2 locus{std::floor(cameraPos.x / displ),
                    std::floor(cameraPos.z / displ)};
    for (auto hmNode = m_heightmapCache.begin();
         hmNode != m_heightmapCache.end();) {
        glm::vec2 nodePos(hmNode->first.first, hmNode->first.second);
        if (std::abs(glm::length(locus - nodePos)) > 15) {
            hmNode = m_heightmapCache.erase(hmNode);
        } else {
            ++hmNode;
        }
    }
}

void TerrainManager::CacheHeightMap(const int x, const int y,
                                    utils::NoiseMap && heightMap) {
    if (m_heightmapCache.size() > 900) {
        PruneHeightMapCache();
    }
    m_heightmapCache[{x, y}] = heightMap;
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
            m_availableBufs.push_back(std::move(chunk->m_meshData));
        }
        removeQueueLkRef.first.get().clear();
    }
    std::vector<std::shared_ptr<UploadReq>> uploadReqs;
    {
        auto uploadQueueLkRef = m_chunkUploadReqs.Lock();
        auto & upreqs = uploadQueueLkRef.first.get();
        uploadReqs.reserve(upreqs.size());
        std::copy(upreqs.begin(), upreqs.end(), std::back_inserter(uploadReqs));
    }
    for (auto & req : uploadReqs) {
        const auto chunkSize = TerrainChunk::GetSidelength();
        float displ = TerrainChunk::vertSpacing * chunkSize;
        const int x = req->index.first;
        const int y = req->index.second;
        glm::vec3 createPos{x * displ, 0, y * displ};
        auto chunk =
            std::make_shared<TerrainChunk>(createPos, m_heightmapCache[{x, y}]);
        if (!m_availableBufs.empty()) {
            chunk->m_meshData = std::move(m_availableBufs.back());
            m_availableBufs.pop_back();
        } else {
            DynamicVBO vbo(TerrainChunk::GetVertexCount() * sizeof(VertexPN));
            chunk->m_meshData = std::move(vbo);
        }
        std::vector<VertexPN> data;
        for (size_t i = 0; i < req->vertices.size(); ++i) {
            data.push_back({req->vertices[i], req->normals[i]});
        }
        chunk->m_meshData.SetSubData(
            0, TerrainChunk::GetVertexCount() * sizeof(VertexPN), data.data());
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

void TerrainManager::Reset() {
    m_chunkUploadReqs.Lock().first.get().clear();
    m_chunks.Lock().first.get().clear();
}
}
