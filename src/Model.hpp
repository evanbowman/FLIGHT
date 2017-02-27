#pragma once

#include <vector>
#include <OpenGL/gl3.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <array>
#include "Vertex.hpp"
#include "BB.hpp"
#include "Shader.hpp"

class Model {
public:
    static std::shared_ptr<Model> LoadFromWavefront(const std::string & path);
    virtual size_t Bind(ShaderProgram & shader) = 0;
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
    size_t Bind(ShaderProgram & shader) override;
};

class ModelPT : public Model {
    friend class Model;
    static std::shared_ptr<Model> LoadFromWavefront(std::fstream &);
public:
    ModelPT(const std::vector<VertexPT> & data, const AABB & aabb);
    size_t Bind(ShaderProgram & shader) override;
};

class ModelPN : public Model {
    friend class Model;
    static std::shared_ptr<Model> LoadFromWavefront(std::fstream &);
public:
    ModelPN(const std::vector<VertexPN> & data, const AABB & aabb);
    size_t Bind(ShaderProgram & shader) override;
};

class ModelPTN : public Model {
    friend class Model;
    static std::shared_ptr<Model> LoadFromWavefront(std::fstream &);
public:
    ModelPTN(const std::vector<VertexPTN> & data, const AABB & aabb);
    size_t Bind(ShaderProgram & shader) override;
};
