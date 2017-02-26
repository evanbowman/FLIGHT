#pragma once

#include <vector>
#include <OpenGL/gl3.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <array>
#include "Vertex.hpp"
#include "BB.hpp"

class Model {
public:
    static std::shared_ptr<Model> LoadFromWavefront(const std::string & path);
    virtual size_t Bind(const GLuint shaderProg) = 0;
    virtual ~Model();
    const AABB & GetAABB() const;
protected:
    Model(const AABB & aabb);
    GLuint m_vbo;
    size_t m_vbLen;
    AABB m_aabb;
};

class ModelP : public Model {
public:
    ModelP(const std::vector<VertexP> & data, const AABB & aabb);
    size_t Bind(const GLuint shaderProg) override;
};

class ModelPT : public Model {
public:
    ModelPT(const std::vector<VertexPT> & data, const AABB & aabb);
    size_t Bind(const GLuint shaderProg) override;    
};

class ModelPN : public Model {
public:
    ModelPN(const std::vector<VertexPN> & data, const AABB & aabb);
    size_t Bind(const GLuint shaderProg) override;
};

class ModelPTN : public Model {
public:
    ModelPTN(const std::vector<VertexPTN> & data, const AABB & aabb);
    size_t Bind(const GLuint shaderProg) override;
};
