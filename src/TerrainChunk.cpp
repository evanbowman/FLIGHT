#include "TerrainChunk.hpp"

#include <SFML/Window.hpp>

namespace FLIGHT {
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

TerrainChunk::TerrainChunk(const glm::vec3 & position)
    : m_drawQuality(TerrainChunk::DrawQuality::None), m_meshData{} {
    m_position = position;
}

void TerrainChunk::Display(ShaderProgram & shader) {
    if (m_drawQuality == DrawQuality::None) {
        return;
    }
    glm::mat4 model;
    model = glm::translate(model, m_position);
    glm::mat4 invTransModel = glm::transpose(glm::inverse(model));
    shader.SetUniformMat4("invTransModel", invTransModel);
    shader.SetUniformMat4("model", model);
    glBindBuffer(GL_ARRAY_BUFFER, m_meshData);
    shader.SetVertexAttribPtr("position", 3, GL_FLOAT, sizeof(VertexPN));
    shader.SetVertexAttribPtr("normal", 3, GL_FLOAT, sizeof(VertexPN),
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

AABB TerrainChunk::GetAABB() {
    static const float MAX_HEIGHTMAP_VALUE = 1.46094;
    const float maxY = MAX_HEIGHTMAP_VALUE * vertElevationScale;
    const float span = vertSpacing * GetSidelength();
    glm::vec3 max{m_position.x + span, maxY, m_position.z};
    glm::vec3 min{m_position.x, 0.f, m_position.z - span};
    return {min, max};
}

void TerrainChunk::OnCollide(Solid & solid) {
    // FIXME: TerrainChunk AABBs are currently broken
}
}
