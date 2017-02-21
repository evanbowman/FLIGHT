#pragma once

enum class TextureId {
    Wing, Engine, Fuselage, Strut, Sun, Count
};

enum class ModelId {
    RWing, LWing, Engine, Fuselage, Strut, Count
};

enum class MaterialId {
    Shellac, Metal, Count
};

enum class ShaderProgramId {
    Base, Shadow, Terrain, Generic, GenericTextured, Font, LensFlare, Count
};

enum class FontId {
    MuseoSlab700, Count
};
