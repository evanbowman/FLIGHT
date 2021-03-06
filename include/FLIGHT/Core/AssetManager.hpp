#pragma once

#include <array>
#include <unordered_map>

#include <FLIGHT/Graphics/Font.hpp>
#include "LoadManifest.hpp"
#include <FLIGHT/Graphics/Material.hpp>
#include <FLIGHT/Graphics/Model.hpp>
#include "RID.hpp"
#include <FLIGHT/Util/ResourcePath.hpp>
#include <FLIGHT/Graphics/Shader.hpp>
#include <FLIGHT/Graphics/Texture.hpp>
#include "Powerup.hpp"

namespace FLIGHT {
enum class FontSize { Small, Medium, Large, Count };

class AssetManager {
private:
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
    std::unordered_map<std::string, std::shared_ptr<Model>> m_models;
    std::unordered_map<std::string, std::shared_ptr<Material>> m_materials;
    std::array<std::shared_ptr<FontFace>, static_cast<int>(FontSize::Count)>
        m_fonts;
    std::array<std::shared_ptr<Texture>, static_cast<int>(Powerup::Count)>
        m_powerupIcons;
    std::array<std::shared_ptr<ShaderProgram>,
               static_cast<int>(ShaderProgramId::Count)>
        m_shaderPrograms;

    void LoadResources();

    void LoadMaterial(const std::string & name);

    void LoadPowerupIcon(const std::string & fname, Powerup id);
    
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
#ifndef NDEBUG
        assert(m_shaderPrograms[static_cast<int>(id)] not_eq nullptr);
#endif
        return *m_shaderPrograms[static_cast<int>(id)];
    }

    template <Powerup id>
    std::shared_ptr<Texture> GetPowerupIcon() {
        return std::get<static_cast<int>(id)>(m_powerupIcons);
    }
    
    template <ShaderProgramId id>
    std::shared_ptr<ShaderProgram> GetProgramPtr() {
#ifndef NDEBUG
        assert(m_shaderPrograms[static_cast<int>(id)] not_eq nullptr);
#endif
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
