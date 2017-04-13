#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <FLIGHT/Core/Font.hpp>
#include <FLIGHT/Core/Time.hpp>
#include <FLIGHT/Graphics/DisplayImpl.hpp>

namespace FLIGHT {
namespace UI {
    class Element {
    public:
	virtual ~Element() = 0;
    };

    class Document {
	std::shared_ptr<Element> GetElementById(const std::string & id);
	void Display(DisplayImpl & renderer);
    };

    class Shape : public Element {
	
    };

    class Text : public Element {
	
    };
}
}
