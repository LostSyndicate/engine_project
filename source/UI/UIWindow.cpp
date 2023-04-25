//
// Created by losts_n1cs3jj on 11/6/2022.
//

#include "UI/UIWindow.h"

UIWindow::~UIWindow()
{
    for (auto& elem : elements_)
        elem->Unref();
}

void UIWindow::AddToView(UIElement* element)
{
    if (element == nullptr || element == this) return;
    bool found = false;
    for (auto& elem : elements_) {
        if (elem == element) {
            found = true;
            break;
        }
    }
    if (!found) {
        element->Ref();
        elements_.push_back(element);
    }
}

void UIWindow::RemoveFromView(UIElement* element)
{
    if (element == nullptr) return;
    for (auto i = 0; i < elements_.size(); i++) {
        if (elements_[i] == element) {
            element->Unref();
            elements_.erase(elements_.begin() + i);
            break;
        }
    }
}

void UIWindow::Render(UICanvas* canvas, Input* input)
{
    glm::vec2 pos = GetVisiblePosition();
    float soff_y = 0.f;    
    if (size_.x > 0 && size_.y > 0)
    {
        canvas->style.color = RGBA_2_HEX(64, 64, 64, 255);
        canvas->AddRect(pos.x, pos.y, size_.x, size_.y);
        canvas->style.color = RGBA_2_HEX(50, 50, 50, 255);
        canvas->AddRectOutline(pos.x, pos.y, size_.x, size_.y);
        
        glm::vec2 v_max = pos;
        for (auto& elem : elements_)
        {
            glm::vec2 e_pos = elem->GetPosition() + pos;
            glm::vec2 e_size = elem->GetSize();
            if (e_pos.x + e_size.x > v_max.x)
                v_max.x = e_pos.x + e_size.x;
            if (e_pos.y + e_size.y > v_max.y)
                v_max.y = e_pos.y + e_size.y;
        }
        int mx, my;
        input->GetMousePos(&mx, &my);    
        if (v_max.y > pos.y + size_.y)
        {
            static const float scrollbar_size = 10.f;
            
            float sbh = (size_.y / (v_max.y - pos.y)) * size_.y;
            glm::vec2 sb_pos = glm::vec2(pos.x + size_.x - scrollbar_size, pos.y + scrollbar_offset_.y);

            canvas->style.color = RGBA_2_HEX(90, 90, 90, 255);
            if (TestPoint(mx, my, sb_pos.x, sb_pos.y, scrollbar_size, sbh))
            {
                canvas->style.color = RGBA_2_HEX(110, 110, 110, 255);
                if (input->IsMouseDown(SM_MOUSE_LEFT))
                {
                    is_grabbing_scrollbar_y_ = true;
                    scrollbar_grab_offset_.y = static_cast<float>(my) - sb_pos.y;
                }
            }
            int whlx, whly;
            input->GetScrollWheel(&whlx, &whly);
            if (!is_grabbing_scrollbar_y_ && whly != 0)
                scrollbar_offset_.y += static_cast<float>(whly) * 15.f;
            if (is_grabbing_scrollbar_y_ && input->IsMouseUp(SM_MOUSE_LEFT))
                is_grabbing_scrollbar_y_ = false;
            if (is_grabbing_scrollbar_y_)
            {
                canvas->style.color = 0xFF7F7F7F;
                scrollbar_offset_.y = static_cast<float>(my) - pos.y - scrollbar_grab_offset_.y;
            }
            if (scrollbar_offset_.y + sbh > size_.y)
                scrollbar_offset_.y = size_.y - sbh;
            if (scrollbar_offset_.y < 0)
                scrollbar_offset_.y = 0;
            soff_y = (scrollbar_offset_.y / size_.y) * (v_max.y - pos.y);

            canvas->AddRect(sb_pos.x, sb_pos.y, scrollbar_size, sbh);

            canvas->style.color = RGBA_2_HEX(50, 50, 50, 255);
            canvas->AddRectOutline(sb_pos.x, sb_pos.y, scrollbar_size, sbh);
        }
        canvas->ResetStyle();
    }
    bool hovering = IsMouseHovered(input);
    for (auto& elem : elements_)
    {
        bool lpe = elem->pointer_events_;
        if (!hovering && size_.x > 0 && size_.y > 0)
            elem->pointer_events_ = false;
        elem->offset_ = glm::vec2(pos.x, pos.y + -soff_y);
        elem->Render(canvas, input);
        if (!hovering)
            elem->pointer_events_ = lpe;
    }
}