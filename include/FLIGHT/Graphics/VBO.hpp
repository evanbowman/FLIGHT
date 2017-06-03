#pragma once

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <cstddef>

namespace FLIGHT {
class VBOBase {
private:
    size_t m_size;
    
protected:
    GLuint m_id;
    void SetSize(const size_t size);
    
public:
    size_t GetSize() const;
};

class StaticVBO : public VBOBase {
public:
    
};

class DynamicVBO : public VBOBase {
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
