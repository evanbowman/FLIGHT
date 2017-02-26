#include "Model.hpp"

std::shared_ptr<Model> Model::LoadFromWavefront(const std::string & path) {
    std::vector<VertexPTN> vertexDataPTN;
    std::vector<VertexPT> vertexDataPT;
    std::vector<VertexPN> vertexDataPN;
    std::vector<VertexP> vertexDataP;
    std::vector<glm::vec3> tempVertices;
    std::vector<glm::vec3> tempNormals;
    std::vector<glm::vec2> tempTexCoords;
    std::fstream file(path);
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
	}
	// NOTE: This assumes that the v, vn, and vt come before the face
	// definitions in the file. Perhaps the wavefront spec allows this, but
	// I have never seen a wavefront where the faces came before the vertex
	// definitions. In any event, this method works for loading models made
	// in Blender, which is the program I use. --E.B.
	else if (prefix == "f ") {
	    if (!tempNormals.empty() && !tempTexCoords.empty()) {
		std::array<int, 3> v;
		std::array<int, 3> t;
		std::array<int, 3> n;
		int resCount = sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d",
				      &v[0], &t[0], &n[0],
				      &v[1], &t[1], &n[1],
				      &v[2], &t[2], &n[2]);
		if (resCount == 9) {
		    for (int i = 0; i < 3; ++i) {
			vertexDataPTN.push_back({
				tempVertices[v[i] - 1],
				tempNormals[n[i] - 1],
				tempTexCoords[t[i] - 1]
			    });
		    }
		}
	    } else if (!tempNormals.empty()) {
		std::array<int, 3> v;
		std::array<int, 3> n;
		int resCount = sscanf(line.c_str(), "f %d//%d %d//%d %d//%d",
				      &v[0], &n[0], &v[1], &n[1], &v[2], &n[2]);
		if (resCount == 6) {
		    for (int i = 0; i < 3; ++i) {
			vertexDataPN.push_back({
				tempVertices[v[i] - 1],
				tempNormals[n[i] - 1]
			    });
		    }
		}
	    } else if (!tempTexCoords.empty()) {
		std::array<int, 3> v;
		std::array<int, 3> t;
		int resCount = sscanf(line.c_str(), "f %d/%d/ %d/%d/ %d/%d/",
				      &v[0], &t[0], &v[1], &t[1], &v[2], &t[2]);
		if (resCount == 6) {
		    for (int i = 0; i < 3; ++i) {
			vertexDataPT.push_back({
				tempVertices[v[i] - 1],
				tempTexCoords[t[i] - 1]
			    });
		    }
		}
	    } else {
		std::array<int, 3> v;
		int resCount = sscanf(line.c_str(), "f %d// %d// %d//",
				      &v[0], &v[1], &v[2]);
		if (resCount == 3) {
		    for (int i = 0; i < 3; ++i) {
			vertexDataP.push_back({
				tempVertices[v[i] - 1]
			    });
		    }
		}
	    }
	}
    }
    glm::vec3 min = tempVertices[0];
    glm::vec3 max = tempVertices[0];
    for (auto & point : tempVertices) {
	min.x = std::min(point.x, min.x);
	min.y = std::min(point.y, min.y);
	min.z = std::min(point.z, min.z);
	max.x = std::max(point.x, max.x);
	max.y = std::max(point.y, max.y);
	max.z = std::max(point.z, max.z);
    }
    AABB aabb(min, max);
    std::shared_ptr<Model> ret = nullptr;
    if (!vertexDataPTN.empty()) {
	ret = std::make_shared<ModelPTN>(vertexDataPTN, aabb);
    } else if (!vertexDataPT.empty()) {
	ret = std::make_shared<ModelPT>(vertexDataPT, aabb);
    } else if (!vertexDataPN.empty()) {
	ret = std::make_shared<ModelPN>(vertexDataPN, aabb);
    } else if (!vertexDataP.empty()) {
	ret = std::make_shared<ModelP>(vertexDataP, aabb);
    }
    return ret;
}

Model::Model(const AABB & aabb) {
    m_aabb = aabb;
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
}

ModelP::ModelP(const std::vector<VertexP> & data, const AABB & aabb) : Model(aabb) {
    m_vbLen = data.size();
    glBufferData(GL_ARRAY_BUFFER, m_vbLen * sizeof(VertexP), data.data(), GL_STATIC_DRAW);
}

ModelPT::ModelPT(const std::vector<VertexPT> & data, const AABB & aabb) : Model(aabb) {
    m_vbLen = data.size();
    glBufferData(GL_ARRAY_BUFFER, m_vbLen * sizeof(VertexPT), data.data(), GL_STATIC_DRAW);
}

ModelPN::ModelPN(const std::vector<VertexPN> & data, const AABB & aabb) : Model(aabb) {
    m_vbLen = data.size();
    glBufferData(GL_ARRAY_BUFFER, m_vbLen * sizeof(VertexPN), data.data(), GL_STATIC_DRAW);
}

ModelPTN::ModelPTN(const std::vector<VertexPTN> & data, const AABB & aabb) : Model(aabb) {
    m_vbLen = data.size();
    glBufferData(GL_ARRAY_BUFFER, m_vbLen * sizeof(VertexPTN), data.data(), GL_STATIC_DRAW);
}

size_t ModelP::Bind(const GLuint shaderProg) {
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    GLint posLoc = glGetAttribLocation(shaderProg, "position");
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    return m_vbLen;
}

size_t ModelPN::Bind(const GLuint shaderProg) {
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    GLint posLoc = glGetAttribLocation(shaderProg, "position");
    GLint normLoc = glGetAttribLocation(shaderProg, "normal");
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPN), 0);
    glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPN), (void *)(sizeof(glm::vec3)));
    return m_vbLen;
}

size_t ModelPT::Bind(const GLuint shaderProg) {
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    GLint posLoc = glGetAttribLocation(shaderProg, "position");
    GLint texLoc = glGetAttribLocation(shaderProg, "texCoord");
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPT), 0);
    glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPT), (void *)(sizeof(glm::vec3)));
    return m_vbLen;
}

size_t ModelPTN::Bind(const GLuint shaderProg) {
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    GLint posLoc = glGetAttribLocation(shaderProg, "position");
    GLint texLoc = glGetAttribLocation(shaderProg, "texCoord");
    GLint normLoc = glGetAttribLocation(shaderProg, "normal");
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), 0);
    glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (void *)(2 * sizeof(glm::vec3)));
    glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (void *)(sizeof(glm::vec3)));
    return m_vbLen;
}

Model::~Model() {
    glDeleteBuffers(1, &m_vbo);
}

const AABB & Model::GetAABB() const {
    return m_aabb;
}
