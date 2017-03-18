#include "UI.hpp"

namespace FLIGHT {
namespace UI {
void Widget::SetMargin(const Margin & margin) { m_margin = margin; }

const Margin & Widget::GetMargin() const { return m_margin; }

const glm::ivec2 & Widget::GetSize() const { return m_size; }

void Widget::SetPosition(const glm::ivec2 & position) { m_position = position; }

void LinearLayout::AddChild(WidgetRef child) {
    m_children.push_back(std::move(child));
}

void VerticalLayout::Update(const Time dt) {
    size_t pen = 0;
    for (auto & child : m_children) {
        pen += child->GetSize().y + child->GetMargin().top;
        child->SetPosition({m_position.x + m_margin.left,
                            (m_position.y - m_margin.top) - pen});
        pen += child->GetMargin().bottom;
        child->Update(dt);
    }
}

void LinearLayout::Display() {
    for (auto & child : m_children) {
        child->Display();
    }
}

void TextView::Update(const Time dt) {
    m_text.SetPosition({m_position.x, m_position.y, 0.f});
    m_size = m_text.GetSize();
}

void TextView::Display() { m_text.Display(); }

Text & TextView::GetText() { return m_text; }
}
}
