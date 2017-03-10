#include "AssetManager.hpp"

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
                                       {"vertex"});
    SetupShader<ShaderProgramId::Reticle>(resPath + "shaders/Reticle.vert",
                                          resPath + "shaders/Reticle.frag",
                                          {"position", "texCoord"});
    SetupShader<ShaderProgramId::ReticleShadow>(
        resPath + "shaders/Reticle.vert",
        resPath + "shaders/ReticleShadow.frag", {"position", "texCoord"});
    SetupShader<ShaderProgramId::SolidColor3D>(
        resPath + "shaders/SolidColor3D.vert",
        resPath + "shaders/SolidColor3D.frag", {"position"});
    SetMaterial<MaterialId::Shellac>({0.8f, 0.84f, 64.f});
    SetMaterial<MaterialId::Metal>({0.8f, 0.5f, 84.f});
}
}
