#include "RedTail.hpp"
#include "Game.hpp"

RedTail::RedTail() {
    Sprite leftWing, rightWing, engine, rightStabilizer, leftStabilizer;
    leftWing.SetTexture(GetGame().GetAssetMgr().Get<TextureId::Wing>());
    rightWing.SetTexture(GetGame().GetAssetMgr().Get<TextureId::Wing>());
    leftWing.SetModel(GetGame().GetAssetMgr().Get<ModelId::LWing>());
    rightWing.SetModel(GetGame().GetAssetMgr().Get<ModelId::RWing>());
    engine.SetModel(GetGame().GetAssetMgr().Get<ModelId::Engine>());
    engine.SetTexture(GetGame().GetAssetMgr().Get<TextureId::Engine>());
    m_fuselage.SetTexture(GetGame().GetAssetMgr().Get<TextureId::Fuselage>());
    m_fuselage.SetModel(GetGame().GetAssetMgr().Get<ModelId::Fuselage>());
    m_fuselage.SetPosition({0, -0.3, -0.3});
    leftWing.SetPosition({-0.84, 0, 0});
    rightWing.SetPosition({0.84, 0, 0});
    leftWing.SetScale({1, 1, -1});
    rightWing.SetScale({1, 1, -1});
    engine.SetScale({1, 1, -1});
    engine.SetPosition({0.f, 0.f, 0.22f});
    leftWing.SetMaterial(GetGame().GetAssetMgr().Get<MaterialId::Shellac>());
    rightWing.SetMaterial(GetGame().GetAssetMgr().Get<MaterialId::Shellac>());
    m_fuselage.SetMaterial(GetGame().GetAssetMgr().Get<MaterialId::Shellac>());
    engine.SetMaterial(GetGame().GetAssetMgr().Get<MaterialId::Metal>());
    leftStabilizer.SetTexture(GetGame().GetAssetMgr().Get<TextureId::Fuselage>());
    rightStabilizer.SetTexture(GetGame().GetAssetMgr().Get<TextureId::Fuselage>());
    leftStabilizer.SetModel(GetGame().GetAssetMgr().Get<ModelId::LWing>());
    rightStabilizer.SetModel(GetGame().GetAssetMgr().Get<ModelId::RWing>());
    leftStabilizer.SetMaterial(GetGame().GetAssetMgr().Get<MaterialId::Shellac>());
    rightStabilizer.SetMaterial(GetGame().GetAssetMgr().Get<MaterialId::Shellac>());
    leftStabilizer.SetScale({0.45, 0.45, -0.45});
    rightStabilizer.SetScale({0.45, 0.45, -0.45});
    leftStabilizer.SetPosition({-0.9, -0.3, -2.4});
    rightStabilizer.SetPosition({0.9, -0.3, -2.4});
    m_components.push_back(leftWing);
    m_components.push_back(rightWing);
    m_components.push_back(engine);
    m_components.push_back(rightStabilizer);
    m_components.push_back(leftStabilizer);
    std::array<Sprite, 4> struts;
    for (auto & strut : struts) {
	strut.SetTexture(GetGame().GetAssetMgr().Get<TextureId::Strut>());
	strut.SetModel(GetGame().GetAssetMgr().Get<ModelId::Strut>());
	strut.SetMaterial(GetGame().GetAssetMgr().Get<MaterialId::Metal>());
    }
    struts[0].SetRotation({0, glm::radians(25.f), glm::radians(60.f)});
    struts[0].SetPosition({-0.30, 0.28, -0.12});
    struts[1].SetRotation({0, glm::radians(-25.f), glm::radians(-60.f)});
    struts[1].SetPosition({0.30, 0.28, -0.12});
    struts[2].SetRotation({0, glm::radians(-12.5f), glm::radians(55.f)});
    struts[2].SetPosition({-0.37, 0.32, -0.12});
    struts[3].SetRotation({0, glm::radians(12.5), glm::radians(-55.f)});
    struts[3].SetPosition({0.37, 0.32, -0.12});
    for (auto & strut : struts) {
	m_components.push_back(strut);
    }
}

void RedTail::Display(ShaderProgram & shader) {
    Plane::Display(shader);
    glm::mat4 modelMatrix;
    modelMatrix = glm::translate(modelMatrix, m_position);
    modelMatrix = glm::rotate(modelMatrix, m_rotation.y, {0, 1, 0});
    modelMatrix = glm::rotate(modelMatrix, m_rotation.z, {0, 0, 1});
    modelMatrix = glm::rotate(modelMatrix, m_rotation.x, {1, 0, 0});
    // I cannot figure out why the fuselage model I made has mismatched winding and
    // normals. Since I can't fix it, momentarily swap the face culling
    glCullFace(GL_BACK);
    m_fuselage.Display(modelMatrix, shader);
    glCullFace(GL_FRONT);
}
