#pragma once

#include <vector>
#include <glm/glm.hpp>
#ifdef FLIGHT_MAC
#include <OpenGL/gl3.h>
#elif FLIGHT_WINDOWS
#include <GL/glew.h>
#endif

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
