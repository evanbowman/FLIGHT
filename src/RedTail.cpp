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
    m_fuselage.SetPosition({0, -0.32, 0.65});
    m_leftWing.SetPosition({0.84, 0, 0});
    m_rightWing.SetPosition({0.84, 0, 0});
    m_leftWing.SetScale({-1, 1, -1});
    m_rightWing.SetScale({1, 1, -1});
    m_engine.SetScale({1, 1, -1});
    m_engine.SetPosition({0.f, 0.f, 0.22f});
}
