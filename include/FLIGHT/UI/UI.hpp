#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <FLIGHT/Core/Font.hpp>
#include <FLIGHT/Core/Time.hpp>

namespace FLIGHT {
namespace UI {
struct Margin {
    size_t top, left, bottom, right;
};

class Widget {
protected:
    Margin m_margin;
    glm::ivec2 m_size;
    glm::ivec2 m_position;

public:
    Widget() : m_margin{} {}
    void SetMargin(const Margin & margin);
    const Margin & GetMargin() const;
    virtual void Display() = 0;
    virtual void Update(const Time dt) = 0;
    const glm::ivec2 & GetSize() const;
    void SetPosition(const glm::ivec2 & position);
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
    void Update(const Time dt) override;
    Text & GetText();
};

class LinearLayout : public Container {
protected:
    std::vector<WidgetRef> m_children;

public:
    void Display() override;
    void AddChild(WidgetRef child) override;
};

class VerticalLayout : public LinearLayout {
public:
    void Update(const Time dt) override;
};

class HorizontalLayout : public LinearLayout {
public:
};
}
}
