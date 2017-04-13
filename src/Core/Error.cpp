#include <FLIGHT/Core/Error.hpp>

void AssertGLStatus(const std::string & context) {
    GLenum err = glGetError();
    if (err not_eq GL_NO_ERROR) {
        std::stringstream ss;
        ss << "GL error in " << context << ", code: " << std::hex << err
           << std::dec;
        while ((err = glGetError()) not_eq GL_NO_ERROR) {
            ss << std::endl
               << "GL error in " << context << ", code: " << std::hex << err
               << std::dec;
        }
        throw std::runtime_error(ss.str());
    }
}
