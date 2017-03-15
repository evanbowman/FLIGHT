#include "UI.hpp"

namespace FLIGHT {
namespace UI {
void LinearLayout::AddChild(WidgetRef child) {
    m_children.push_back(std::move(child));
}

void VerticalLayout::Display() {
    size_t pen = 0;
    for (auto & child : m_children) {
        child->SetPosition({m_position.x, m_position.y - pen});
        child->Display();
	pen += child->GetSize().y;
    }
}

void VerticalLayout::SetPosition(const glm::ivec2 & position) {
    m_position = position;
}

glm::ivec2 VerticalLayout::GetSize() {
    throw std::runtime_error("Unimplemented method GetSize()");
}

void HorizontalLayout::SetPosition(const glm::ivec2 & position) {
    m_position = position;
}

void HorizontalLayout::Display() {
    throw std::runtime_error("HorizontalLayout::Display() unimplemented");
}

glm::ivec2 HorizontalLayout::GetSize() {
    throw std::runtime_error("Unimplemented method GetSize()");
}

void TextView::Display() { m_text.Display(); }

Text & TextView::GetText() { return m_text; }

void TextView::SetPosition(const glm::ivec2 & position) {
    m_text.SetPosition({position.x, position.y, 0.f});
}

glm::ivec2 TextView::GetSize() { return m_text.GetSize(); }
}
}
