#pragma once

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <array>
#include <cassert>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <FLIGHT/Core/Error.hpp>

namespace FLIGHT {
class ShaderProgram {
    ShaderProgram() {}
    GLuint m_handle;
    std::unordered_map<std::string, GLint> m_uniformLocs;
    GLint GetUniformLoc(const std::string &);

public:
    static std::shared_ptr<ShaderProgram>
    New(const std::string & vertShaderPath, const std::string & fragShaderPath);
    void Use();
    void EnableAttribs(const std::vector<std::string> & attribs);
    void SetUniformMat4(const std::string & name, const glm::mat4 & mat);
    void SetUniformVec4(const std::string & name, const glm::vec4 & vec);
    void SetUniformVec3(const std::string & name, const glm::vec3 & vec);
    void SetUniformFloat(const std::string & name, const float f);
    void SetUniformInt(const std::string & name, const int i);
    void SetVertexAttribPtr(const std::string & name, GLint size, GLenum type,
                            GLsizei stride = 0, size_t offset = 0);
};
}
