#include "Model.hpp"

#include "AssetManager.hpp"

void Model::LoadFromWavefront(const std::string & path) {
    std::vector<Vertex> vertexData;
    std::vector<glm::vec3> tempVertices;
    std::vector<glm::vec3> tempNormals;
    std::vector<glm::vec2> tempTexCoords;
    std::fstream file(path);
    size_t linum = 0;
    std::string line;
    while (std::getline(file, line)) {
	std::string prefix;
	std::copy(line.begin(), line.begin() + 2, std::back_inserter(prefix));
	std::stringstream ss(line);
	if (prefix == "v ") {
	    float x, y, z;
	    if (ss >> prefix >> x >> y >> z) {
		tempVertices.push_back({x, y, z});
	    }
	} else if (prefix == "vn") {
	    float x, y, z;
	    if (ss >> prefix >> x >> y >> z) {
		tempNormals.push_back({x, y, z});
	    }
	} else if (prefix == "vt") {
	    float x, y;
	    if (ss >> prefix >> x >> y) {
		tempTexCoords.push_back({x, y});
	    }
	} else if (prefix == "f ") {
	    std::array<int, 3> v;
	    std::array<int, 3> t;
	    int n;
	    int resCount = sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d",
				  &v[0], &t[0], &n,
				  &v[1], &t[1], &n,
				  &v[2], &t[2], &n);
	    if (resCount != 9) {
		std::cerr << linum << std::endl;
		throw std::runtime_error("Invalid format, failed to load: " + path);
	    }
	    for (int i = 0; i < 3; ++i) {
		vertexData.push_back({
			tempVertices[v[i] - 1],
			tempNormals[n - 1],
			tempTexCoords[t[i] - 1]
		    });
	    }
	}
	++linum;
    }
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    m_vertCoordSize = vertexData.size() * sizeof(Vertex);
    glBufferData(GL_ARRAY_BUFFER,
		 m_vertCoordSize,
		 vertexData.data(), GL_STATIC_DRAW);
}

size_t Model::Bind(const GLuint shaderProgram) {
    if (m_vbo == 0) {
	throw std::runtime_error("Model binding failed: vertex buffer does not exist");
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			  (void *)(2 * sizeof(glm::vec3)));
    return m_vertCoordSize;
}
