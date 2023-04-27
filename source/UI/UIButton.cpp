//
// Created by losts_n1cs3jj on 11/6/2022.
//

#include "UI/UIButton.h"

UIButton::~UIButton() {}

void UIButton::Render(UICanvas* canvas, Input* input)
{
    glm::vec2 pos = GetVisiblePosition();
    
    just_pressed_ = false;
    bool hovering = IsMouseHovered(input) && pointer_events_;
    if (hovering && input->IsMouseDown(SM_MOUSE_LEFT))
    {
        just_pressed_ = true;
        pressed_ = true;
        SetFocus(true);
    }
    if (input->IsMouseUp(SM_MOUSE_LEFT))
    {
        pressed_ = false;
        SetFocus(false);
    }
    glm::vec2 text_size = font_->ComputeSize(text_.c_str());

    float text_offset = 0.f;
    canvas->style.gradient.type = Gradient::TO_TOP;
    canvas->style.gradient.a = RGBA_2_HEX(52, 52, 52, 255);
    canvas->style.gradient.b = RGBA_2_HEX(90, 90, 90, 255);
    if (hovering)
    {
        canvas->style.gradient.a = RGBA_2_HEX(72, 72, 72, 255);
        canvas->style.gradient.b = RGBA_2_HEX(110, 110, 110, 255);
    }
    if (pressed_)
    {
        canvas->style.gradient.a = RGBA_2_HEX(70, 70, 70, 255);
        canvas->style.gradient.b = RGBA_2_HEX(45, 45, 45, 255);
        text_offset = 1.f;
    }
    canvas->AddRect(pos.x, pos.y, size_.x, size_.y);
    
    canvas->style.gradient.type = Gradient::NONE;
    canvas->style.color = RGBA_2_HEX(50, 50, 50, 255);
    canvas->AddRectOutline(pos.x, pos.y, size_.x, size_.y);
    
    if (!pressed_)
    {
        canvas->style.color = RGBA_2_HEX(255, 255, 255, 80);
        canvas->AddLine(pos.x + 1.f, pos.y + size_.y - 1.f, pos.x + size_.x - 1.f, pos.y + size_.y - 1.f);
    }
        
    canvas->style.color = 0xFFFFFFFF;
    canvas->style.z_order = 1;
    glm::vec2 text_pos = pos + CalcTextOffset(text_.c_str());
    canvas->AddText(font_, text_pos.x + text_offset, text_pos.y - text_offset, text_.c_str());
    //canvas->AddText(font_, pos.x + (size_.x / 2.f) - (text_size.x / 2.f) + text_offset, pos.y + (size_.y / 2.f) - (text_size.y / 2.f) - text_offset, text_.c_str());
    canvas->ResetStyle();
    
    UIElement::Render(canvas, input);
}
