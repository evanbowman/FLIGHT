#include "RedTail.hpp"

RedTail::RedTail() {
    m_leftWing.SetTexture(GetAssets().GetTexture(TextureId::Wing));
    m_rightWing.SetTexture(GetAssets().GetTexture(TextureId::Wing));
    m_leftWing.SetModel(GetAssets().GetModel(ModelId::Wing));
    m_rightWing.SetModel(GetAssets().GetModel(ModelId::Wing));
    m_engine.SetModel(GetAssets().GetModel(ModelId::Engine));
    m_engine.SetTexture(GetAssets().GetTexture(TextureId::Engine));
    m_fuselage.SetTexture(GetAssets().GetTexture(TextureId::Fuselage));
    m_fuselage.SetModel(GetAssets().GetModel(ModelId::Fuselage));
    m_fuselage.SetPosition({0, -0.3, -0.3});
    m_leftWing.SetPosition({0.84, 0, 0});
    m_rightWing.SetPosition({0.84, 0, 0});
    m_leftWing.SetScale({-1, 1, -1});
    m_rightWing.SetScale({1, 1, -1});
    m_engine.SetScale({1, 1, -1});
    m_engine.SetPosition({0.f, 0.f, 0.22f});
    static const Material planeShell{0.8, 0.6, 64};
    m_leftWing.SetMaterial(planeShell);
    m_rightWing.SetMaterial(planeShell);
    m_fuselage.SetMaterial(planeShell);
    m_engine.SetMaterial({0.8, 0.5, 84});
    m_leftStabilizer.SetTexture(GetAssets().GetTexture(TextureId::Fuselage));
    m_rightStabilizer.SetTexture(GetAssets().GetTexture(TextureId::Fuselage));
    m_leftStabilizer.SetModel(GetAssets().GetModel(ModelId::Wing));
    m_rightStabilizer.SetModel(GetAssets().GetModel(ModelId::Wing));
    m_leftStabilizer.SetMaterial(planeShell);
    m_rightStabilizer.SetMaterial(planeShell);
    m_leftStabilizer.SetScale({-0.45, 0.45, -0.45});
    m_rightStabilizer.SetScale({0.45, 0.45, -0.45});
    m_leftStabilizer.SetPosition({0.9, -0.3, -2.3});
    m_rightStabilizer.SetPosition({0.9, -0.3, -2.3});
}
