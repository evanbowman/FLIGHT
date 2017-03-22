#include "AssetManager.hpp"
#include "Game.hpp"

namespace FLIGHT {
void AssetManager::LoadResources() {
    auto resPath = ResourcePath();
    auto manifestData = LoadManifest();
    for (const auto & textureName : manifestData.textures) {
        LoadTexture(textureName);
    }
    for (const auto & modelName : manifestData.models) {
        LoadModel(modelName);
    }
    for (const auto & materialName : manifestData.materials) {
        LoadMaterial(materialName);
    }
    LoadFont(GetGame().GetConf().localization.font);
    SetupShader<ShaderProgramId::SkyGradient>(
        resPath + "shaders/SkyGradient.vert",
        resPath + "shaders/SkyGradient.frag", {"position"});
    SetupShader<ShaderProgramId::Shadow>(resPath + "shaders/depth.vert",
                                         resPath + "shaders/depth.frag",
                                         {"position"});
    SetupShader<ShaderProgramId::GenericTextured>(
        resPath + "shaders/genericTextured.vert",
        resPath + "shaders/genericTextured.frag", {"position", "texCoord"});
    SetupShader<ShaderProgramId::Terrain>(resPath + "shaders/terrain.vert",
                                          resPath + "shaders/terrain.frag",
                                          {"position", "normal"});
    SetupShader<ShaderProgramId::Base>(resPath + "shaders/base.vert",
                                       resPath + "shaders/base.frag",
                                       {"position", "texCoord", "normal"});
    SetupShader<ShaderProgramId::LensFlare>(resPath + "shaders/LensFlare.vert",
                                            resPath + "shaders/LensFlare.frag",
                                            {"position"});
    SetupShader<ShaderProgramId::Generic>(resPath + "shaders/generic.vert",
                                          resPath + "shaders/generic.frag",
                                          {"position"});
    SetupShader<ShaderProgramId::Font>(resPath + "shaders/Font.vert",
                                       resPath + "shaders/Font.frag",
                                       {"position", "texCoord"});
    SetupShader<ShaderProgramId::Reticle>(resPath + "shaders/Reticle.vert",
                                          resPath + "shaders/Reticle.frag",
                                          {"position", "texCoord"});
    SetupShader<ShaderProgramId::ReticleShadow>(
        resPath + "shaders/Reticle.vert",
        resPath + "shaders/ReticleShadow.frag", {"position", "texCoord"});
    SetupShader<ShaderProgramId::SolidColor3D>(
        resPath + "shaders/SolidColor3D.vert",
        resPath + "shaders/SolidColor3D.frag", {"position"});
}

void AssetManager::LoadMaterial(const std::string & name) {
    auto materialSp = std::make_shared<Material>();
    *materialSp = {};
    std::ifstream file(ResourcePath() + "materials/" + name);
    std::stringstream ss;
    ss << file.rdbuf();
    YAML::Node node = YAML::Load(ss.str());
    if (auto diffuse = node["diffuse"]) {
        materialSp->diffuse = diffuse.as<float>();
    }
    if (auto spec = node["specular"]) {
        materialSp->specular = spec.as<float>();
    }
    if (auto shininess = node["shininess"]) {
        materialSp->shininess = shininess.as<float>();
    }
    m_materials[name] = materialSp;
}

void AssetManager::LoadTexture(const std::string & name,
                               Texture::Sampling sampling) {
    auto textureSp = std::make_shared<Texture>();
    textureSp->LoadFromFile(ResourcePath() + "textures/" + name, sampling);
    m_textures[name] = textureSp;
}

void AssetManager::LoadFont(const std::string & name) {
    auto avgWindowSize =
        (GetGame().GetWindowSize().x + GetGame().GetWindowSize().y) / 2;
    std::array<double, static_cast<int>(FontSize::Count)> sizes{0.025, 0.042,
                                                                0.057};
    std::get<0>(m_fonts) = FontFace::New(ResourcePath() + "fonts/" + name,
                                         std::get<0>(sizes) * avgWindowSize);
    std::get<1>(m_fonts) = FontFace::New(ResourcePath() + "fonts/" + name,
                                         std::get<1>(sizes) * avgWindowSize);
    std::get<2>(m_fonts) = FontFace::New(ResourcePath() + "fonts/" + name,
                                         std::get<2>(sizes) * avgWindowSize);
}

void AssetManager::LoadModel(const std::string & name) {
    auto modelSp = Model::LoadFromWavefront(ResourcePath() + "models/" + name);
    m_models[name] = modelSp;
}
}
