#include "MeshBuilder.hpp"

MeshBuilder::MeshBuilder(size_t width, size_t height) {
    m_vertices.resize(width * height);
    m_normals.resize(width * height);
    m_triangles.reserve((width - 1) * (height - 1) * 6);
}

void MeshBuilder::AddVertex(size_t vertexIndex, const glm::vec3 & vert) {
    m_vertices[vertexIndex] = vert;
}

void MeshBuilder::AddTriangle(GLushort a, GLushort b, GLushort c) {
    m_triangles.push_back(a);
    m_triangles.push_back(b);
    m_triangles.push_back(c);
}

Mesh MeshBuilder::GetMesh() { return {m_triangles, m_vertices}; }
