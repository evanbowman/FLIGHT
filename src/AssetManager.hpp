#pragma once

#include <array>
#include <unordered_map>

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
class AssetManager {
private:
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
    std::unordered_map<std::string, std::shared_ptr<Model>> m_models;
    std::array<std::shared_ptr<Material>, static_cast<int>(ModelId::Count)>
        m_materials;
    std::array<std::shared_ptr<ShaderProgram>,
               static_cast<int>(ShaderProgramId::Count)>
        m_shaderPrograms;

    void LoadResources();

    template <MaterialId id> void SetMaterial(const Material & material) {
        auto sp = std::make_shared<Material>();
        *sp = material;
        std::get<static_cast<int>(id)>(m_materials) = sp;
    }

    void LoadTexture(const std::string & name,
                     Texture::Sampling sampling = Texture::Sampling::Nearest) {
        auto textureSp = std::make_shared<Texture>();
        textureSp->LoadFromFile(ResourcePath() + "textures/" + name, sampling);
        m_textures[name] = textureSp;
    }

    void LoadModel(const std::string & name) {
        auto modelSp =
            Model::LoadFromWavefront(ResourcePath() + "models/" + name);
        m_models[name] = modelSp;
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

    template <ShaderProgramId id> std::shared_ptr<ShaderProgram> GetProgram() {
        assert(m_shaderPrograms[static_cast<int>(id)] != nullptr);
        return m_shaderPrograms[static_cast<int>(id)];
    }

    template <MaterialId id> std::shared_ptr<Material> GetMaterial() {
        assert(m_materials[static_cast<int>(id)] != nullptr);
        return m_materials[static_cast<int>(id)];
    }

    std::shared_ptr<Texture> GetTexture(const std::string & name) {
        return m_textures[name];
    }

    std::shared_ptr<Model> GetModel(const std::string & name) {
        return m_models[name];
    }
};
}
