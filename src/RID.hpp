#pragma once

enum class TextureId {
    Wing, Engine, Fuselage, Strut, Sun, Count
};

enum class ModelId {
    RWing, LWing, Engine, Fuselage, Strut, Mountain, Count
};

enum class MaterialId {
    Shellac, Metal, Count
};

enum class ShaderProgramId {
    Base, Shadow, Terrain, GenericTextured, LensFlare, Count
};
