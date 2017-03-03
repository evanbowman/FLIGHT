#pragma once

#include "Plane.hpp"

namespace FLIGHT {   
    class RedTail : public Plane {
	Sprite m_fuselage;
    public:
	RedTail();
	void Display(ShaderProgram & shader) override;
    };
}
