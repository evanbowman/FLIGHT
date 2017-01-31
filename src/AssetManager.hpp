#pragma once

#include <array>
#include "ResourcePath.hpp"
#include "Texture.hpp"
#include "Model.hpp"
#include "RID.hpp"

class AssetManager {
private:
    std::array<std::shared_ptr<Texture>, static_cast<int>(TextureId::Count)> m_textures;
    std::array<std::shared_ptr<Model>, static_cast<int>(ModelId::Count)> m_models;
    std::array<GLuint, static_cast<int>(ShaderProgramId::Count)> m_shaderPrograms;
    
    void LoadResources();
    void CreateProgram(const GLuint vert, const GLuint frag, ShaderProgramId id);
    GLuint SetupShader(const std::string & path, GLenum shaderType);

    inline void LoadTexture(const std::string & path, TextureId id) {
	auto textureSp = std::make_shared<Texture>();
	textureSp->LoadFromFile(path);
	m_textures[static_cast<int>(id)] = textureSp;
    }

    inline void LoadModel(const std::string & path, ModelId id) {
	auto modelSp = std::make_shared<Model>();
	modelSp->LoadFromWavefront(path);
	m_models[static_cast<int>(id)] = modelSp;
    }
    
public:
    friend class App;
    
    inline GLuint GetShaderProgram(ShaderProgramId id) {
	return m_shaderPrograms[static_cast<int>(id)];
    }
    
    std::shared_ptr<Texture> GetTexture(TextureId id) {
	assert(m_textures[static_cast<int>(id)] != nullptr);
	return m_textures[static_cast<int>(id)];
    }

    std::shared_ptr<Model> GetModel(ModelId id) {
	assert(m_models[static_cast<int>(id)] != nullptr);
	return m_models[static_cast<int>(id)];
    }
};

AssetManager & GetAssets();
