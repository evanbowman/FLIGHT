#include "MeshBuilder.hpp"

MeshBuilder::MeshBuilder(size_t width, size_t height) {
    m_vertices.resize(width * height);
    m_normals.resize(width * height);
    m_triangles.resize((width - 1) * (height - 1) * 6);
    m_triangleIndex = 0;
}

void MeshBuilder::AddVertex(size_t vertexIndex, const glm::vec3 & vert) {
    m_vertices[vertexIndex] = vert;
}

void MeshBuilder::AddTriangle(GLushort a, GLushort b, GLushort c) {
    m_triangles[m_triangleIndex++] = a;
    m_triangles[m_triangleIndex++] = b;
    m_triangles[m_triangleIndex++] = c;
}

Mesh MeshBuilder::GetMesh() {
    return {m_triangles, m_vertices};
}
