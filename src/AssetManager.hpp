/// @file
/// @brief Asset Manager class definition

#pragma once

#include <array>
#include <unordered_map>
#include <yaml-cpp/yaml.h>

#include "Error.hpp"
#include "Font.hpp"
#include "LoadManifest.hpp"
#include "Material.hpp"
#include "Model.hpp"
#include "RID.hpp"
#include "ResourcePath.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

namespace FLIGHT {
enum class FontSize { Small, Medium, Large, Count };

class AssetManager {
private:
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
    std::unordered_map<std::string, std::shared_ptr<Model>> m_models;
    std::unordered_map<std::string, std::shared_ptr<Material>> m_materials;
    std::array<std::shared_ptr<FontFace>, static_cast<int>(FontSize::Count)>
        m_fonts;
    std::array<std::shared_ptr<ShaderProgram>,
               static_cast<int>(ShaderProgramId::Count)>
        m_shaderPrograms;

    void LoadResources();

    void LoadMaterial(const std::string & name);

    void LoadTexture(const std::string & name,
                     Texture::Sampling sampling = Texture::Sampling::Nearest);

    void LoadFont(const std::string & name);

    void LoadModel(const std::string & name);

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

    template <ShaderProgramId id> ShaderProgram & GetProgram() {
        assert(m_shaderPrograms[static_cast<int>(id)] != nullptr);
        return *m_shaderPrograms[static_cast<int>(id)];
    }

    template <ShaderProgramId id>
    std::shared_ptr<ShaderProgram> GetProgramPtr() {
        assert(m_shaderPrograms[static_cast<int>(id)] != nullptr);
        return m_shaderPrograms[static_cast<int>(id)];
    }

    template <FontSize size> std::shared_ptr<FontFace> GetFont() {
        return std::get<static_cast<int>(size)>(m_fonts);
    }

    std::shared_ptr<Material> GetMaterial(const std::string & name) {
        return m_materials[name];
    }

    std::shared_ptr<Texture> GetTexture(const std::string & name) {
        return m_textures[name];
    }

    std::shared_ptr<Model> GetModel(const std::string & name) {
        return m_models[name];
    }
};
}
