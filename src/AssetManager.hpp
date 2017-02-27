#pragma once

#include <array>
#include "ResourcePath.hpp"
#include "Material.hpp"
#include "Texture.hpp"
#include "Model.hpp"
#include "RID.hpp"
#include "Error.hpp"
#include "Font.hpp"
#include "Shader.hpp"

class AssetManager {
private:
    std::array<std::shared_ptr<Texture>, static_cast<int>(TextureId::Count)> m_textures;
    std::array<std::shared_ptr<Model>, static_cast<int>(ModelId::Count)> m_models;
    std::array<std::shared_ptr<Material>, static_cast<int>(ModelId::Count)> m_materials;
    std::array<std::shared_ptr<FontFace>, static_cast<int>(FontId::Count)> m_fonts;
    std::array<std::shared_ptr<ShaderProgram>, static_cast<int>(ShaderProgramId::Count)> m_shaderPrograms;
    
    void LoadResources();
    
    template <MaterialId id>
    void SetMaterial(const Material & material) {
	auto sp = std::make_shared<Material>();
	*sp = material;
	std::get<static_cast<int>(id)>(m_materials) = sp;
    }

    template <FontId id>
    void LoadFont(const std::string & path) {
	auto fontSp = std::make_shared<FontFace>();
	fontSp->CreateFromFile(path, 72);
	std::get<static_cast<int>(id)>(m_fonts) = fontSp;
    }

    template <TextureId id>
    void LoadTexture(const std::string & path) {
	auto textureSp = std::make_shared<Texture>();
	textureSp->LoadFromFile(path);
	std::get<static_cast<int>(id)>(m_textures) = textureSp;
    }

    template <ModelId id>
    void LoadModel(const std::string & path) {
	auto modelSp = Model::LoadFromWavefront(path);
	std::get<static_cast<int>(id)>(m_models) = modelSp;
    }

    template <ShaderProgramId id>
    void SetupShader(const std::string & vertShaderPath,
		     const std::string & fragShaderPath,
		     const std::vector<std::string> & attribs) {
	auto shaderSp = ShaderProgram::New(vertShaderPath, fragShaderPath);
	std::get<static_cast<int>(id)>(m_shaderPrograms) = shaderSp;
	shaderSp->Use();
	shaderSp->EnableAttribs(attribs);
    }
    
public:
    friend class Game;
    
    std::shared_ptr<ShaderProgram> GetShaderProgram(ShaderProgramId id) {
	assert(m_shaderPrograms[static_cast<int>(id)] != nullptr);
	return m_shaderPrograms[static_cast<int>(id)];
    }

    std::shared_ptr<FontFace> GetFont(FontId id) {
	assert(m_fonts[static_cast<int>(id)] != nullptr);
	return m_fonts[static_cast<int>(id)];
    }

    std::shared_ptr<Material> GetMaterial(MaterialId id) {
	assert(m_materials[static_cast<int>(id)] != nullptr);
	return m_materials[static_cast<int>(id)];
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
