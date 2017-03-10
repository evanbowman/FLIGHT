#include "RedTail.hpp"
#include "Game.hpp"

namespace FLIGHT {
RedTail::RedTail() {
    Sprite leftWing, rightWing, engine, rightStabilizer, leftStabilizer;
    leftWing.SetTexture(GetGame().GetAssetMgr().GetTexture("RTWing.png"));
    rightWing.SetTexture(GetGame().GetAssetMgr().GetTexture("RTWing.png"));
    leftWing.SetModel(GetGame().GetAssetMgr().GetModel("RTLWing.obj"));
    rightWing.SetModel(GetGame().GetAssetMgr().GetModel("RTRWing.obj"));
    engine.SetModel(GetGame().GetAssetMgr().GetModel("RTEngine.obj"));
    engine.SetTexture(GetGame().GetAssetMgr().GetTexture("RTEngine.png"));
    m_fuselage.SetTexture(GetGame().GetAssetMgr().GetTexture("RTFuselage.png"));
    m_fuselage.SetModel(GetGame().GetAssetMgr().GetModel("RTFuselage.obj"));
    m_fuselage.SetPosition({0, -0.3, -0.3});
    leftWing.SetPosition({-0.84, 0, 0});
    rightWing.SetPosition({0.84, 0, 0});
    leftWing.SetScale({1, 1, -1});
    rightWing.SetScale({1, 1, -1});
    engine.SetScale({1, 1, -1});
    engine.SetPosition({0.f, 0.f, 0.22f});
    leftWing.SetMaterial(
        GetGame().GetAssetMgr().GetMaterial<MaterialId::Shellac>());
    rightWing.SetMaterial(
        GetGame().GetAssetMgr().GetMaterial<MaterialId::Shellac>());
    m_fuselage.SetMaterial(
        GetGame().GetAssetMgr().GetMaterial<MaterialId::Shellac>());
    engine.SetMaterial(
        GetGame().GetAssetMgr().GetMaterial<MaterialId::Metal>());
    leftStabilizer.SetTexture(
        GetGame().GetAssetMgr().GetTexture("RTFuselage.png"));
    rightStabilizer.SetTexture(
        GetGame().GetAssetMgr().GetTexture("RTFuselage.png"));
    leftStabilizer.SetModel(GetGame().GetAssetMgr().GetModel("RTLWing.obj"));
    rightStabilizer.SetModel(GetGame().GetAssetMgr().GetModel("RTRWing.obj"));
    leftStabilizer.SetMaterial(
        GetGame().GetAssetMgr().GetMaterial<MaterialId::Shellac>());
    rightStabilizer.SetMaterial(
        GetGame().GetAssetMgr().GetMaterial<MaterialId::Shellac>());
    leftStabilizer.SetScale({0.45, 0.45, -0.45});
    rightStabilizer.SetScale({0.45, 0.45, -0.45});
    leftStabilizer.SetPosition({-0.9, -0.3, -2.4});
    rightStabilizer.SetPosition({0.9, -0.3, -2.4});
    m_parts.push_back(leftWing);
    m_parts.push_back(rightWing);
    m_parts.push_back(engine);
    m_parts.push_back(rightStabilizer);
    m_parts.push_back(leftStabilizer);
    std::array<Sprite, 4> struts;
    for (auto & strut : struts) {
        strut.SetTexture(GetGame().GetAssetMgr().GetTexture("Strut.png"));
        strut.SetModel(GetGame().GetAssetMgr().GetModel("Strut.obj"));
        strut.SetMaterial(
            GetGame().GetAssetMgr().GetMaterial<MaterialId::Metal>());
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
        m_parts.push_back(strut);
    }
    m_mbsRadius = MBS(GetAABB()).GetRadius();
}

void RedTail::Display(ShaderProgram & shader) {
    Plane::Display(shader);
    glm::mat4 modelMatrix;
    modelMatrix = glm::translate(modelMatrix, m_position);
    modelMatrix = glm::rotate(modelMatrix, m_rotation.y, {0, 1, 0});
    modelMatrix = glm::rotate(modelMatrix, m_rotation.z, {0, 0, 1});
    modelMatrix = glm::rotate(modelMatrix, m_rotation.x, {1, 0, 0});
    // I cannot figure out why the fuselage model I made has mismatched winding
    // and
    // normals. Since I can't fix it, momentarily swap the face culling
    glCullFace(GL_BACK);
    m_fuselage.Display(modelMatrix, shader);
    glCullFace(GL_FRONT);
}

OBB RedTail::GetOBB() {
    AABB aabb = m_fuselage.GetAABB();
    for (auto & comp : m_parts) {
        aabb.Merge(comp.GetAABB());
    }
    OBB obb(aabb);
    obb.Rotate(m_rotation.y, {0, 1, 0});
    obb.Rotate(m_rotation.z, {0, 0, 1});
    obb.Rotate(m_rotation.x, {1, 0, 0});
    obb.Translate(m_position);
    return obb;
}

AABB RedTail::GetAABB() {
    AABB aabb = Plane::GetAABB();
    aabb.Merge(m_fuselage.GetAABB());
    return aabb;
}
}
