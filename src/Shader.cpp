#include "Shader.hpp"

namespace FLIGHT {    
    void ShaderProgram::Use() { glUseProgram(m_handle); }

    GLint ShaderProgram::GetUniformLoc(const std::string & name) {
	auto loc = m_uniformLocs.find(name);
	if (loc != m_uniformLocs.end()) {
	    return loc->second;
	} else {
	    const GLint uniLoc = glGetUniformLocation(m_handle, name.c_str());
	    if (uniLoc == -1) {
		throw std::runtime_error("Uniform \'" + name + "\' does not exist");
	    }
	    m_uniformLocs[name] = uniLoc;
	    return uniLoc;
	}
    }

    void ShaderProgram::SetVertexAttribPtr(const std::string & name, GLint size,
					   GLenum type, GLsizei stride,
					   size_t offset) {
	const GLint attribLoc = glGetAttribLocation(m_handle, name.c_str());
	glVertexAttribPointer(attribLoc, size, type, GL_FALSE, stride,
			      (void *)offset);
    }

    void ShaderProgram::SetUniformVec3(const std::string & name,
				       const glm::vec3 & vec) {
	glUniform3f(GetUniformLoc(name), vec.x, vec.y, vec.z);
    }

    void ShaderProgram::SetUniformVec4(const std::string & name,
				       const glm::vec4 & vec) {
	glUniform4f(GetUniformLoc(name), vec.x, vec.y, vec.z, vec.w);
    }

    void ShaderProgram::SetUniformMat4(const std::string & name,
				       const glm::mat4 & mat) {
	glUniformMatrix4fv(GetUniformLoc(name), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void ShaderProgram::SetUniformFloat(const std::string & name, const float f) {
	glUniform1f(GetUniformLoc(name), f);
    }

    void ShaderProgram::SetUniformInt(const std::string & name, const int i) {
	glUniform1i(GetUniformLoc(name), i);
    }

    void ShaderProgram::EnableAttribs(const std::vector<std::string> & attribs) {
	for (auto & attrib : attribs) {
	    const GLint attribLoc = glGetAttribLocation(m_handle, attrib.c_str());
	    if (attribLoc == -1) {
		throw std::runtime_error("Attribute \'" + attrib + "\' DNE");
	    }
	    glEnableVertexAttribArray(attribLoc);
	    AssertGLStatus("Enabling attribute " + attrib);
	}
    }

    static const size_t ERR_LOG_BUFFER_SIZE = 1024;

    static GLuint CompileShader(const std::string & path, GLenum shaderType) {
	std::ifstream ifs(path);
	std::stringstream buffer;
	buffer << ifs.rdbuf();
	GLuint shader = glCreateShader(shaderType);
	auto content = buffer.str();
	const auto src = content.c_str();
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);
	GLint test;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &test);
	if (!test) {
	    std::array<char, ERR_LOG_BUFFER_SIZE> compilationLog{};
	    glGetShaderInfoLog(shader, compilationLog.size(), nullptr,
			       compilationLog.data());
	    std::string logStr(compilationLog.data());
	    throw std::runtime_error("Failed to compile " + path + " due to: " +
				     logStr);
	}
	return shader;
    }

    std::shared_ptr<ShaderProgram>
    ShaderProgram::New(const std::string & vertShaderPath,
		       const std::string & fragShaderPath) {
	auto program = std::shared_ptr<ShaderProgram>(new ShaderProgram());
	auto vert = CompileShader(vertShaderPath, GL_VERTEX_SHADER);
	auto frag = CompileShader(fragShaderPath, GL_FRAGMENT_SHADER);
	program->m_handle = glCreateProgram();
	glAttachShader(program->m_handle, vert);
	glAttachShader(program->m_handle, frag);
	glDeleteShader(vert);
	glDeleteShader(frag);
	glLinkProgram(program->m_handle);
	std::array<char, ERR_LOG_BUFFER_SIZE> buffer{};
	GLsizei dummyLen;
	glGetProgramInfoLog(program->m_handle, buffer.size(), &dummyLen,
			    buffer.data());
	if (buffer[0]) {
	    std::string logStr(buffer.data());
	    throw std::runtime_error("Failed to link " + vertShaderPath + " and " +
				     fragShaderPath + "due to: " + logStr);
	}
	return program;
    }
}
