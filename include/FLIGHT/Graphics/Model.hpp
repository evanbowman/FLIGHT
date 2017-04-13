#pragma once

#include <vector>
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <FLIGHT/Core/BB.hpp>
#include "Shader.hpp"
#include "Vertex.hpp"
#include <array>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

namespace FLIGHT {
class Model {
public:
    struct Binding {
        size_t numVertices;
        explicit Binding(const size_t numVertices) {
            this->numVertices = numVertices;
        }
        Binding(const Binding &) = delete;
        Binding(Binding && other) { numVertices = other.numVertices; }
        ~Binding() { glBindBuffer(GL_ARRAY_BUFFER, 0); }
    };
    static std::shared_ptr<Model> LoadFromWavefront(const std::string & path);
    virtual Binding Bind(ShaderProgram & shader) = 0;
    virtual ~Model();
    const AABB & GetAABB() const;

protected:
    Model(const AABB & aabb);
    GLuint m_vbo;
    size_t m_vbLen;
    AABB m_aabb;
};

class ModelP : public Model {
    friend class Model;
    static std::shared_ptr<Model> LoadFromWavefront(std::fstream &);

public:
    ModelP(const std::vector<VertexP> & data, const AABB & aabb);
    Binding Bind(ShaderProgram & shader) override;
};

class ModelPT : public Model {
    friend class Model;
    static std::shared_ptr<Model> LoadFromWavefront(std::fstream &);

public:
    ModelPT(const std::vector<VertexPT> & data, const AABB & aabb);
    Binding Bind(ShaderProgram & shader) override;
};

class ModelPN : public Model {
    friend class Model;
    static std::shared_ptr<Model> LoadFromWavefront(std::fstream &);

public:
    ModelPN(const std::vector<VertexPN> & data, const AABB & aabb);
    Binding Bind(ShaderProgram & shader) override;
};

class ModelPTN : public Model {
    friend class Model;
    static std::shared_ptr<Model> LoadFromWavefront(std::fstream &);

public:
    ModelPTN(const std::vector<VertexPTN> & data, const AABB & aabb);
    Binding Bind(ShaderProgram & shader) override;
};
}
