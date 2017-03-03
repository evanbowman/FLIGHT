#include "Error.hpp"

void AssertGLStatus(const std::string & context) {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::stringstream ss;
        ss << "GL error in " << context << ", code: " << std::hex << err
           << std::dec;
        while ((err = glGetError()) != GL_NO_ERROR) {
            ss << std::endl
               << "GL error in " << context << ", code: " << std::hex << err
               << std::dec;
        }
        throw std::runtime_error(ss.str());
    }
}
