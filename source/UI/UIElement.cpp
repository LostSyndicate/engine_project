//
// Created by losts_n1cs3jj on 11/6/2022.
//

#include "UI/UIElement.h"
#include <stdint.h>

static UIElement* g_Focused = nullptr;

UIElement::~UIElement()
{
    if (g_Focused == this) g_Focused = nullptr;
}

UIElement* UIElement::GetFocused()
{
    return g_Focused;
}

bool UIElement::TestPoint(int px, int py,
                      float rx, float ry, float rw, float rh)
{
    return (float)px >= rx && (float)px <= rx + rw &&
            (float)py >= ry && (float)py <= ry + rh;
}

void UIElement::RenderTooltip(UICanvas* canvas, Input* input, Font* font, const char* tooltip)
{
    if (font == nullptr)
        return;
        
    static const float y_offset = 32.f;
    static const glm::vec2 padding = glm::vec2(10.f, 5.f);

    int mx, my;
    input->GetMousePos(&mx, &my);
    int sw, sh;
    input->GetWindowSize(&sw, &sh);
    
    glm::vec2 text_size = font->ComputeSize(tooltip);
    glm::vec2 size = text_size + (padding * 2.f);
    glm::vec2 pos = glm::vec2(mx, my - text_size.y - y_offset);
    
    if (my - size.y - y_offset <= 0)
        pos.y = my + 5.f;
    if (my >= sh)
        pos.y = sh - size.y;
    if (mx + size.x >= sw)
        pos.x = sw - size.x;
    if (mx <= 0)
        pos.x = 0;
    
    canvas->style.color = RGBA_2_HEX(50, 50, 50, 255);
    canvas->style.z_order = INT32_MAX - 1;
    canvas->AddRect(pos.x - 1.f, pos.y - 1.f, size.x + 2.f, size.y + 2.f);
    
    canvas->style.color = RGBA_2_HEX(72, 72, 72, 255);
    canvas->AddRect(pos.x, pos.y, size.x, size.y);
    
    canvas->style.color = 0xFFFFFFFF;
    canvas->style.z_order = INT32_MAX;
    canvas->AddText(font, pos.x + padding.x, pos.y + padding.y, tooltip);
    
    canvas->ResetStyle();
}

glm::vec2 UIElement::AnchorToValue(Anchor anchor)
{
    glm::vec2 value = glm::vec2(0.5f, 0.5f);
    if (anchor & A_RIGHT)  { value.x += 0.5f; }
    if (anchor & A_LEFT)   { value.x -= 0.5f; }
    if (anchor & A_TOP)    { value.y += 0.5f; }
    if (anchor & A_BOTTOM) { value.y -= 0.5f; }
    return value;
}

void UIElement::SetAnchor(glm::vec2 anchor)
{
    anchor_ = anchor;
}

glm::vec2 UIElement::GetAnchor() const
{
    return anchor_;
}

void UIElement::SetAlignment(glm::vec2 anchor)
{
    alignment_ = anchor;
    content_offset_ = size_ * anchor;
}

glm::vec2 UIElement::GetAlignment() const
{
    return alignment_;
}

void UIElement::SetTextAlignment(Anchor pos)
{
    text_align_ = pos;
}

UIElement::Anchor UIElement::GetTextAlignment() const
{
    return text_align_;
}

void UIElement::SetPosition(const glm::vec2 &position)
{
    position_ = position;
}

glm::vec2 UIElement::GetPosition() const
{
    return position_;
}

glm::vec2 UIElement::GetVisiblePosition() const
{
    return position_ + offset_;
}

void UIElement::SetSize(const glm::vec2 &size)
{
    size_ = size;
    content_offset_ = size * alignment_;
}

glm::vec2 UIElement::GetSize() const
{
    return size_;
}

void UIElement::SetFont(Font* font)
{
    font_ = font;
}

Font* UIElement::GetFont() const
{
    return font_;
}

void UIElement::SetText(const std::string_view& text)
{
    text_ = text;
}

std::string UIElement::GetText() const
{
    return text_;
}

bool UIElement::IsFocused() const
{
    return g_Focused == this;
}

void UIElement::SetFocus(bool focus)
{
    if (!focus && g_Focused == this) g_Focused = nullptr;
    else if (focus) g_Focused = this;
}

void UIElement::SetDefaultSize()
{
    glm::vec2 text_size = font_->ComputeSize(text_.c_str());
    size_.x = text_size.x + 15.f;
    size_.y = text_size.y + 10.f;
}

void UIElement::SetTooltip(const std::string_view& tooltip)
{
    tooltip_ = tooltip;
}

std::string UIElement::GetTooltip() const
{
    return tooltip_;
}

void UIElement::EnablePointerEvents(bool enabled)
{
    pointer_events_ = enabled;
}

bool UIElement::IsPointerEventsEnabled() const
{
    return pointer_events_;
}

void UIElement::SetEnabled(bool value)
{
    enabled_ = value;
}

bool UIElement::IsEnabled() const
{
    return enabled_;
}

void UIElement::SetData(void* data)
{
    data_ = data;
}

void* UIElement::GetData() const
{
    return data_;
}

void UIElement::Render(UICanvas* canvas, Input* input)
{
    if (IsMouseHovered(input) && !tooltip_.empty() && pointer_events_)
        RenderTooltip(canvas, input, font_, tooltip_.c_str());
}

bool UIElement::IsMouseHovered(Input* input) const
{
    int mx, my;
    input->GetMousePos(&mx, &my);
    return TestPoint(mx, my, position_.x + offset_.x, position_.y + offset_.y, size_.x, size_.y);
}

glm::vec2 UIElement::CalcTextOffset(const char* text)
{
    if (font_ == nullptr)
        return glm::vec2(0.f);
    glm::vec2 anchor = AnchorToValue(text_align_);
    return size_ * anchor - font_->ComputeSize(text) * anchor;
}