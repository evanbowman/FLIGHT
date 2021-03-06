#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <FLIGHT/Graphics/OpenGL.hpp>

namespace FLIGHT {
struct Mesh {
    std::vector<GLushort> triangles;
    std::vector<glm::vec3> vertices;
};

class MeshBuilder {
    std::vector<GLushort> m_triangles;
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec3> m_normals;

public:
    MeshBuilder(size_t width, size_t height);
    void AddVertex(size_t vertexIndex, const glm::vec3 & vert);
    void AddTriangle(GLushort a, GLushort b, GLushort c);
    Mesh GetMesh();
};
}
