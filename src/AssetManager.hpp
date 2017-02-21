#pragma once

#include <array>
#include "ResourcePath.hpp"
#include "Material.hpp"
#include "Texture.hpp"
#include "Model.hpp"
#include "RID.hpp"
#include "Error.hpp"
#include "Font.hpp"

class AssetManager {
private:
    std::array<std::shared_ptr<Texture>, static_cast<int>(TextureId::Count)> m_textures;
    std::array<std::shared_ptr<Model>, static_cast<int>(ModelId::Count)> m_models;
    std::array<std::shared_ptr<Material>, static_cast<int>(ModelId::Count)> m_materials;
    std::array<std::shared_ptr<FontAtlas>, static_cast<int>(FontId::Count)> m_fonts;
    std::array<GLuint, static_cast<int>(ShaderProgramId::Count)> m_shaderPrograms;
    
    void LoadResources();
    void CreateProgram(const GLuint vert, const GLuint frag, ShaderProgramId id);
    void EnableProgramAttribs(ShaderProgramId id, const std::vector<std::string> & attribs);
    GLuint SetupShader(const std::string & path, GLenum shaderType);

    template <MaterialId id>
    void SetMaterial(const Material & material) {
	auto sp = std::make_shared<Material>();
	*sp = material;
	std::get<static_cast<int>(id)>(m_materials) = sp;
    }

    inline void LoadFont(const std::string & path, FontId id) {
	auto fontSp = std::make_shared<FontAtlas>();
	fontSp->LoadFromFile(path, {
		CalcFontSize(FontSizeFactor::Large),
		CalcFontSize(FontSizeFactor::Medium),
		CalcFontSize(FontSizeFactor::Small)
	    });
	m_fonts[static_cast<int>(id)] = fontSp;
    }

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
    friend class Game;
    
    inline GLuint GetShaderProgram(ShaderProgramId id) {
	return m_shaderPrograms[static_cast<int>(id)];
    }

    std::shared_ptr<Material> GetMaterial(MaterialId id) {
	assert(m_materials[static_cast<int>(id)] != nullptr);
	return m_materials[static_cast<int>(id)];
    }

    std::shared_ptr<FontAtlas> GetFont(FontId id) {
	assert(m_fonts[static_cast<int>(id)] != nullptr);
	return m_fonts[static_cast<int>(id)];
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
