#include "AssetManager.hpp"

#include <OpenGL/glu.h>

void AssetManager::LoadResources() {
    auto resPath = ResourcePath();
    LoadTexture(resPath + "textures/wingTexture.png", TextureId::Wing);
    LoadTexture(resPath + "textures/engineTexture.png", TextureId::Engine);
    LoadModel(resPath + "models/wing.obj", ModelId::Wing);
    LoadModel(resPath + "models/engine.obj", ModelId::Engine);
    CreateProgram(SetupShader(resPath + "shaders/base2.vert", GL_VERTEX_SHADER),
		  SetupShader(resPath + "shaders/base.frag", GL_FRAGMENT_SHADER), ShaderProgramId::Base);
}

void AssetManager::CreateProgram(const GLuint vert, const GLuint frag, ShaderProgramId id) {
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vert);
    glAttachShader(shaderProgram, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);
    glLinkProgram(shaderProgram);
    char buffer[1024];
    GLsizei len;
    glGetProgramInfoLog(shaderProgram, 1024, &len, buffer);
    printf("%s", buffer);
    glUseProgram(shaderProgram);
    m_shaderPrograms[static_cast<int>(id)] = shaderProgram;
}

GLuint AssetManager::SetupShader(const std::string & path, GLenum shaderType) {
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
	std::vector<char> compilationLog(512);
	glGetShaderInfoLog(shader, compilationLog.size(), nullptr,
			   compilationLog.data());
	std::cerr << compilationLog.data() << std::endl;
	exit(1);
    }
    return shader;
}

AssetManager assetManager;

AssetManager & GetAssets() {
    return ::assetManager;
}
