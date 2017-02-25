#pragma once

enum class TextureId {
    Wing, Engine, Fuselage, Strut, Sun, Count
};

enum class ModelId {
    RWing, LWing, Engine, Fuselage, Strut, SkyDome, Count
};

enum class MaterialId {
    Shellac, Metal, Count
};

enum class ShaderProgramId {
    Base, Shadow, Terrain, Generic, GenericTextured, Font, LensFlare, Reticle, ReticleShadow, SkyGradient, Count
};

enum class FontId {
    MuseoSlab700, Count
};
