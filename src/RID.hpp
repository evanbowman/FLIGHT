#pragma once

namespace FLIGHT {    
    enum class TextureId {
	Wing, Engine, Fuselage, Strut, Sun, Vignette, Count
    };

    enum class ModelId {
	RWing, LWing, Engine, Fuselage, Strut, SkyDome, Bullet, Count
    };

    enum class MaterialId {
	Shellac, Metal, Count
    };

    enum class ShaderProgramId {
	Base, Shadow, Terrain, Generic, GenericTextured, Font, SolidColor3D, LensFlare, Reticle, ReticleShadow, SkyGradient, Count
    };

    enum class FontId {
	MuseoSlab700, Count
    };
}
