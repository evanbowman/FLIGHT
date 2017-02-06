#include "RedTail.hpp"

RedTail::RedTail() {
    Sprite leftWing, rightWing, engine, rightStabilizer, leftStabilizer;
    leftWing.SetTexture(GetAssets().GetTexture(TextureId::Wing));
    rightWing.SetTexture(GetAssets().GetTexture(TextureId::Wing));
    leftWing.SetModel(GetAssets().GetModel(ModelId::LWing));
    rightWing.SetModel(GetAssets().GetModel(ModelId::RWing));
    engine.SetModel(GetAssets().GetModel(ModelId::Engine));
    engine.SetTexture(GetAssets().GetTexture(TextureId::Engine));
    m_fuselage.SetTexture(GetAssets().GetTexture(TextureId::Fuselage));
    m_fuselage.SetModel(GetAssets().GetModel(ModelId::Fuselage));
    m_fuselage.SetPosition({0, -0.3, -0.3});
    //fuselage.SetScale({-1, 0, 0});
    leftWing.SetPosition({-0.84, 0, 0});
    rightWing.SetPosition({0.84, 0, 0});
    leftWing.SetScale({1, 1, -1});
    rightWing.SetScale({1, 1, -1});
    engine.SetScale({1, 1, -1});
    engine.SetPosition({0.f, 0.f, 0.22f});
    leftWing.SetMaterial(GetAssets().GetMaterial(MaterialId::Shellac));
    rightWing.SetMaterial(GetAssets().GetMaterial(MaterialId::Shellac));
    m_fuselage.SetMaterial(GetAssets().GetMaterial(MaterialId::Shellac));
    engine.SetMaterial(GetAssets().GetMaterial(MaterialId::Metal));
    leftStabilizer.SetTexture(GetAssets().GetTexture(TextureId::Fuselage));
    rightStabilizer.SetTexture(GetAssets().GetTexture(TextureId::Fuselage));
    leftStabilizer.SetModel(GetAssets().GetModel(ModelId::LWing));
    rightStabilizer.SetModel(GetAssets().GetModel(ModelId::RWing));
    leftStabilizer.SetMaterial(GetAssets().GetMaterial(MaterialId::Shellac));
    rightStabilizer.SetMaterial(GetAssets().GetMaterial(MaterialId::Shellac));
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
	strut.SetTexture(GetAssets().GetTexture(TextureId::Strut));
	strut.SetModel(GetAssets().GetModel(ModelId::Strut));
	strut.SetMaterial(GetAssets().GetMaterial(MaterialId::Metal));
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

void RedTail::Display(const GLuint shaderProgram) {
    Plane::Display(shaderProgram);
    glm::mat4 modelMatrix;
    modelMatrix = glm::translate(modelMatrix, m_position);
    modelMatrix = glm::rotate(modelMatrix, m_rotation.y, {0, 1, 0});
    modelMatrix = glm::rotate(modelMatrix, m_rotation.z, {0, 0, 1});
    modelMatrix = glm::rotate(modelMatrix, m_rotation.x, {1, 0, 0});
    // I cannot figure out why the fuselage model I made has mismatched winding and
    // normals. Since I can't fix it, momentarily swap the face culling
    glCullFace(GL_BACK);
    m_fuselage.Display(modelMatrix, shaderProgram);
    glCullFace(GL_FRONT);
}
