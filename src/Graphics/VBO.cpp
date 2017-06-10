#include <FLIGHT/Graphics/VBO.hpp>
#include <stdexcept>
#include <utility>

namespace FLIGHT {
size_t VBOBase::GetSize() const { return m_size; }

void VBOBase::SetSize(const size_t size) { m_size = size; }

DynamicVBO::DynamicVBO() : m_valid(false) {}

DynamicVBO::DynamicVBO(const size_t size) : m_valid(true) {
    this->SetSize(size);
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

void DynamicVBO::Bind() {
    if (m_valid) {
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
    } else {
        throw std::runtime_error("Attempt to bind invalid buffer");
    }
}

void DynamicVBO::SetSubData(const size_t offset, const size_t len,
                            const GLvoid * data) {
    if (m_valid) {
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
        glBufferSubData(GL_ARRAY_BUFFER, offset, len, data);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

DynamicVBO & DynamicVBO::operator=(DynamicVBO && from) {
    m_id = from.m_id;
    m_valid = from.m_valid;
    from.m_valid = false;
    return *this;
}

DynamicVBO::DynamicVBO(DynamicVBO && from) { *this = std::move(from); }

DynamicVBO::~DynamicVBO() {
    if (m_valid) {
        glDeleteBuffers(1, &m_id);
    }
}
}
