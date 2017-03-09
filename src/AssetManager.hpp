#pragma once

#include "Error.hpp"
#include "Font.hpp"
#include "Material.hpp"
#include "Model.hpp"
#include "RID.hpp"
#include "ResourcePath.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include <array>

namespace FLIGHT {
class AssetManager {
private:
    std::array<std::shared_ptr<Texture>, static_cast<int>(TextureId::Count)>
        m_textures;
    std::array<std::shared_ptr<Model>, static_cast<int>(ModelId::Count)>
        m_models;
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

    template <TextureId id>
    void LoadTexture(const std::string & path,
                     Texture::Sampling sampling = Texture::Sampling::Nearest) {
        auto textureSp = std::make_shared<Texture>();
        textureSp->LoadFromFile(path, sampling);
        std::get<static_cast<int>(id)>(m_textures) = textureSp;
    }

    template <ModelId id> void LoadModel(const std::string & path) {
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

    template <ShaderProgramId id> std::shared_ptr<ShaderProgram> GetProgram() {
        assert(m_shaderPrograms[static_cast<int>(id)] != nullptr);
        return m_shaderPrograms[static_cast<int>(id)];
    }

    template <MaterialId id> std::shared_ptr<Material> GetMaterial() {
        assert(m_materials[static_cast<int>(id)] != nullptr);
        return m_materials[static_cast<int>(id)];
    }

    template <TextureId id> std::shared_ptr<Texture> GetTexture() {
        assert(m_textures[static_cast<int>(id)] != nullptr);
        return m_textures[static_cast<int>(id)];
    }

    template <ModelId id> std::shared_ptr<Model> GetModel() {
        assert(m_models[static_cast<int>(id)] != nullptr);
        return m_models[static_cast<int>(id)];
    }
};
}
