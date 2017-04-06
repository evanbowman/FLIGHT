#pragma once

#ifdef FLIGHT_MAC
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <cstddef>
#include <stdexcept>
#include <utility>

namespace FLIGHT {
class DynamicVBO {
    GLuint m_id;
    bool m_valid;

public:
    DynamicVBO();
    void SetSubData(const size_t offset, const size_t len, const GLvoid * data);
    explicit DynamicVBO(const size_t size);
    DynamicVBO(DynamicVBO && from);
    void Bind();
    ~DynamicVBO();
    DynamicVBO & operator=(DynamicVBO && from);
};
}
