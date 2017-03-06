#include "Model.hpp"

namespace FLIGHT {
std::shared_ptr<Model> Model::LoadFromWavefront(const std::string & path) {
    std::fstream file(path);
    std::string line;
    bool hasPoints = false, hasTexCoords = false, hasNormals = false;
    while (std::getline(file, line)) {
        if (line.find("v ") != std::string::npos) {
            hasPoints = true;
        }
        if (line.find("vn") != std::string::npos) {
            hasNormals = true;
        }
        if (line.find("vt") != std::string::npos) {
            hasTexCoords = true;
        }
        if (hasPoints && hasNormals && hasTexCoords) {
            break;
        }
    }
    file.clear();
    file.seekg(0, std::ios::beg);
    if (hasPoints && hasTexCoords && hasNormals) {
        return ModelPTN::LoadFromWavefront(file);
    } else if (hasPoints && hasTexCoords) {
        return ModelPT::LoadFromWavefront(file);
    } else if (hasPoints && hasNormals) {
        return ModelPN::LoadFromWavefront(file);
    } else if (hasPoints) {
        return ModelP::LoadFromWavefront(file);
    } else {
        throw std::runtime_error(
            "wavefront file " + path +
            " has no vertex position data (lines starting with \'v\')");
    }
}

static AABB CalcAABB(const std::vector<glm::vec3> & vertices) {
    glm::vec3 min = vertices[0];
    glm::vec3 max = vertices[0];
    for (auto & point : vertices) {
        min.x = std::min(point.x, min.x);
        min.y = std::min(point.y, min.y);
        min.z = std::min(point.z, min.z);
        max.x = std::max(point.x, max.x);
        max.y = std::max(point.y, max.y);
        max.z = std::max(point.z, max.z);
    }
    return AABB(min, max);
}

std::shared_ptr<Model> ModelPTN::LoadFromWavefront(std::fstream & file) {
    std::vector<VertexPTN> vertexData;
    std::vector<glm::vec3> tempVertices;
    std::vector<glm::vec3> tempNormals;
    std::vector<glm::vec2> tempTexCoords;
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
            std::array<int, 3> n;
            int resCount =
                sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &v[0],
                       &t[0], &n[0], &v[1], &t[1], &n[1], &v[2], &t[2], &n[2]);
            if (resCount == 9) {
                for (int i = 0; i < 3; ++i) {
                    vertexData.push_back({tempVertices[v[i] - 1],
                                          tempNormals[n[i] - 1],
                                          tempTexCoords[t[i] - 1]});
                }
            }
        }
    }
    return std::make_shared<ModelPTN>(vertexData, CalcAABB(tempVertices));
}

std::shared_ptr<Model> ModelPN::LoadFromWavefront(std::fstream & file) {
    std::vector<VertexPN> vertexData;
    std::vector<glm::vec3> tempVertices;
    std::vector<glm::vec3> tempNormals;
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
        } else if (prefix == "f ") {
            std::array<int, 3> v;
            std::array<int, 3> n;
            int resCount = sscanf(line.c_str(), "f %d//%d %d//%d %d//%d", &v[0],
                                  &n[0], &v[1], &n[1], &v[2], &n[2]);
            if (resCount == 6) {
                for (int i = 0; i < 3; ++i) {
                    vertexData.push_back({
                        tempVertices[v[i] - 1], tempNormals[n[i] - 1],
                    });
                }
            }
        }
    }
    return std::make_shared<ModelPN>(vertexData, CalcAABB(tempVertices));
}

std::shared_ptr<Model> ModelPT::LoadFromWavefront(std::fstream & file) {
    std::vector<VertexPT> vertexData;
    std::vector<glm::vec3> tempVertices;
    std::vector<glm::vec2> tempTexCoords;
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
        } else if (prefix == "vt") {
            float x, y;
            if (ss >> prefix >> x >> y) {
                tempTexCoords.push_back({x, y});
            }
        } else if (prefix == "f ") {
            std::array<int, 3> v;
            std::array<int, 3> t;
            int resCount = sscanf(line.c_str(), "f %d/%d/ %d/%d/ %d/%d/", &v[0],
                                  &t[0], &v[1], &t[1], &v[2], &t[2]);
            if (resCount == 6) {
                for (int i = 0; i < 3; ++i) {
                    vertexData.push_back(
                        {tempVertices[v[i] - 1], tempTexCoords[t[i] - 1]});
                }
            }
        }
    }
    return std::make_shared<ModelPT>(vertexData, CalcAABB(tempVertices));
}

std::shared_ptr<Model> ModelP::LoadFromWavefront(std::fstream & file) {
    std::vector<VertexP> vertexData;
    std::vector<glm::vec3> tempVertices;
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
        } else if (prefix == "f ") {
            std::array<int, 3> v;
            int resCount =
                sscanf(line.c_str(), "f %d// %d// %d//", &v[0], &v[1], &v[2]);
            if (resCount != 3) {
                resCount =
                    sscanf(line.c_str(), "f %d %d %d", &v[0], &v[1], &v[2]);
                if (resCount != 3) {
                    continue;
                }
            }
            for (int i = 0; i < 3; ++i) {
                vertexData.push_back({
                    tempVertices[v[i] - 1],
                });
            }
        }
    }
    return std::make_shared<ModelP>(vertexData, CalcAABB(tempVertices));
}

Model::Model(const AABB & aabb) {
    m_aabb = aabb;
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
}

ModelP::ModelP(const std::vector<VertexP> & data, const AABB & aabb)
    : Model(aabb) {
    m_vbLen = data.size();
    glBufferData(GL_ARRAY_BUFFER, m_vbLen * sizeof(VertexP), data.data(),
                 GL_STATIC_DRAW);
}

ModelPT::ModelPT(const std::vector<VertexPT> & data, const AABB & aabb)
    : Model(aabb) {
    m_vbLen = data.size();
    glBufferData(GL_ARRAY_BUFFER, m_vbLen * sizeof(VertexPT), data.data(),
                 GL_STATIC_DRAW);
}

ModelPN::ModelPN(const std::vector<VertexPN> & data, const AABB & aabb)
    : Model(aabb) {
    m_vbLen = data.size();
    glBufferData(GL_ARRAY_BUFFER, m_vbLen * sizeof(VertexPN), data.data(),
                 GL_STATIC_DRAW);
}

ModelPTN::ModelPTN(const std::vector<VertexPTN> & data, const AABB & aabb)
    : Model(aabb) {
    m_vbLen = data.size();
    glBufferData(GL_ARRAY_BUFFER, m_vbLen * sizeof(VertexPTN), data.data(),
                 GL_STATIC_DRAW);
}

size_t ModelP::Bind(ShaderProgram & shader) {
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    shader.SetVertexAttribPtr("position", 3, GL_FLOAT);
    return m_vbLen;
}

size_t ModelPN::Bind(ShaderProgram & shader) {
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    shader.SetVertexAttribPtr("position", 3, GL_FLOAT, sizeof(VertexPN));
    shader.SetVertexAttribPtr("normal", 3, GL_FLOAT, sizeof(VertexPN),
                              sizeof(glm::vec3));
    return m_vbLen;
}

size_t ModelPT::Bind(ShaderProgram & shader) {
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    shader.SetVertexAttribPtr("position", 3, GL_FLOAT, sizeof(VertexPT));
    shader.SetVertexAttribPtr("texCoord", 2, GL_FLOAT, sizeof(VertexPT),
                              sizeof(glm::vec3));
    return m_vbLen;
}

size_t ModelPTN::Bind(ShaderProgram & shader) {
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    shader.SetVertexAttribPtr("position", 3, GL_FLOAT, sizeof(VertexPTN));
    shader.SetVertexAttribPtr("texCoord", 2, GL_FLOAT, sizeof(VertexPTN),
                              2 * sizeof(glm::vec3));
    shader.SetVertexAttribPtr("normal", 3, GL_FLOAT, sizeof(VertexPTN),
                              sizeof(glm::vec3));
    return m_vbLen;
}

Model::~Model() { glDeleteBuffers(1, &m_vbo); }

const AABB & Model::GetAABB() const { return m_aabb; }
}
