#include "MeshBuilder.hpp"

MeshBuilder::MeshBuilder(size_t width, size_t height) {
    m_vertices.resize(width * height);
    m_normals.resize(width * height);
    m_triangles.resize((width - 1) * (height - 1) * 6);
}

void MeshBuilder::AddVertex(size_t vertexIndex, const glm::vec3 & vert) {
    m_vertices[vertexIndex] = vert;
}

void MeshBuilder::AddTriangle(GLushort a, GLushort b, GLushort c) {
    m_triangles[m_triangleIndex++] = a;
    m_triangles[m_triangleIndex++] = b;
    m_triangles[m_triangleIndex++] = c;
}

void MeshBuilder::CalculateNormals() {
    const size_t vertexCount = m_triangles.size();
    for (size_t i = 0; i < vertexCount; ++i) {
	const glm::vec3 & p0 = m_vertices[m_triangles[i + 0]];
	const glm::vec3 & p1 = m_vertices[m_triangles[i + 1]];
	const glm::vec3 & p2 = m_vertices[m_triangles[i + 2]];
	glm::vec3 e1 = p1 - p0;
	glm::vec3 e2 = p2 - p0;
	glm::vec3 normal = glm::cross(e1, e2);
	normal = glm::normalize(normal);
	m_normals[m_triangles[i + 0]] += normal;
	m_normals[m_triangles[i + 1]] += normal;
	m_normals[m_triangles[i + 2]] += normal;
    }
    for (size_t i = 0; i < m_normals.size(); ++i) {
	m_normals[i] = glm::normalize(m_normals[i]);
    }
}

Mesh MeshBuilder::GetMesh() {
    return {m_triangles, m_vertices, m_normals};
}
