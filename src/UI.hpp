#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "Font.hpp"

namespace FLIGHT {
namespace UI {
struct Margin {
    size_t top, left, bottom, right;
};
    
class Widget {
protected:
    Margin m_margin;
public:
    Widget() : m_margin{} {}
    void SetMargin(const Margin & margin);
    const Margin & GetMargin() const;
    virtual void Display() = 0;
    virtual glm::ivec2 GetSize() = 0;
    virtual void SetPosition(const glm::ivec2 & position) = 0;
    virtual ~Widget() {}
};

using WidgetRef = std::unique_ptr<Widget>;

class Container : public Widget {
public:
    virtual void AddChild(WidgetRef child) = 0;
};

class TextView : public Widget {
    Text m_text;
public:
    void Display() override;
    glm::ivec2 GetSize() override;
    void SetPosition(const glm::ivec2 & position) override;
    Text & GetText();
};

class LinearLayout : public Container {
protected:
    std::vector<WidgetRef> m_children;
    glm::ivec2 m_position;
public:
    void AddChild(WidgetRef child) override;
};

class VerticalLayout : public LinearLayout {
public:
    void SetPosition(const glm::ivec2 & position) override;
    glm::ivec2 GetSize() override;
    void Display() override;
};

class HorizontalLayout : public LinearLayout {
public:
    void SetPosition(const glm::ivec2 & position) override;
    glm::ivec2 GetSize() override;
    void Display() override;
};
}
}
