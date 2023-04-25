//
// Created by losts_n1cs3jj on 11/6/2022.
//

#include "UI/UIDropdown.h"

UIDropdown::~UIDropdown(){}

void UIDropdown::AddOption(const std::string_view &name)
{
    options_.emplace_back(std::pair<std::string, std::string>(std::string(name), std::string()));
}

void UIDropdown::AddOption(const std::string_view& name, const std::string_view& tooltip)
{
    options_.emplace_back(std::pair<std::string, std::string>(std::string(name), std::string(tooltip)));
}

void UIDropdown::RemoveOption(int index)
{
    options_.erase(options_.begin() + index);
}

int UIDropdown::GetOptionCount() const
{
    return static_cast<int>(options_.size());
}

std::string UIDropdown::GetOption(int index) const
{
    return options_[index].first;
}

int UIDropdown::GetSelected() const
{
    return selected_;
}

void UIDropdown::Select(int index)
{
    selected_ = index;
}

void UIDropdown::SetExpandMode(ExpandMode mode)
{
    expand_mode_ = mode;
}

UIDropdown::ExpandMode UIDropdown::GetExpandMode() const
{
    return expand_mode_;
}

void UIDropdown::OnSelectCallback(OnSelectFn fn)
{
    on_select_fn_ = fn;
}

void UIDropdown::OnSelect(int index)
{
    if (on_select_fn_ != nullptr)
        on_select_fn_(this, index);
}

void UIDropdown::Render(UICanvas *canvas, Input *input)
{
    static const float arrow_area_width = 32.f;
    static const float expand_arrow_padding = 8.f;
    
    glm::vec2 pos = GetVisiblePosition();
    
    std::string sel_option = options_[selected_].first;
    glm::vec2 text_size = font_->ComputeSize(sel_option.c_str());
    bool hovering = IsMouseHovered(input);
    
    canvas->style.gradient.type = Gradient::TO_TOP;
    canvas->style.gradient.a = RGBA_2_HEX(54, 54, 54, 255);
    canvas->style.gradient.b = RGBA_2_HEX(40, 40, 40, 255);
    if (hovering && pointer_events_)
    {
        canvas->style.gradient.a = RGBA_2_HEX(54, 54, 54, 255);
        canvas->style.gradient.b = RGBA_2_HEX(60, 60, 60, 255);
    }
    canvas->AddRect(pos.x, pos.y, size_.x, size_.y);
    
    canvas->style.gradient.type = Gradient::NONE;
    canvas->style.color = RGBA_2_HEX(46, 46, 46, 255);
    canvas->AddRect(pos.x + size_.x - arrow_area_width - 1.f, 
                    pos.y, arrow_area_width, size_.y);

    canvas->style.color = RGBA_2_HEX(64, 64, 64, 255);
    canvas->AddRect(pos.x + size_.x - arrow_area_width, 
                    pos.y, arrow_area_width, size_.y);
    
    canvas->style.color = RGBA_2_HEX(46, 46, 46, 255);
    canvas->AddRectOutline(pos.x, pos.y, size_.x, size_.y);
    
    canvas->style.color = 0xFFFFFFFF;
    canvas->style.z_order = 1;
    canvas->AddText(font_, pos.x + 10.f, 
                    pos.y + (size_.y / 2.f) - (text_size.y / 2.f), sel_option.c_str());
    
    const unsigned short tri_inds[3] = {0,1,2};
    Vertex2D tri_verts[3];
    if (opened_ && expand_mode_ == EXPAND_DOWN || !opened_ && expand_mode_ == EXPAND_UP)
    {
        // pointing up
        tri_verts[0] = Vertex2D(glm::vec2(pos.x + size_.x - arrow_area_width + 8.f, pos.y + 8.f), glm::vec2(0.f), 0xFFFFFFFF);
        tri_verts[1] = Vertex2D(glm::vec2(pos.x + size_.x - 8.f, pos.y + 8.f), glm::vec2(0.f), 0xFFFFFFFF);
        tri_verts[2] = Vertex2D(glm::vec2(pos.x + size_.x - (arrow_area_width / 2.f), pos.y + size_.y - 8.f), glm::vec2(0.f), 0xFFFFFFFF);
    }
    else 
    {
        // pointing down
        tri_verts[0] = Vertex2D(glm::vec2(pos.x + size_.x - (arrow_area_width / 2.f), pos.y + expand_arrow_padding ), glm::vec2(0.f), 0xFFFFFFFF);
        tri_verts[1] = Vertex2D(glm::vec2(pos.x + size_.x - expand_arrow_padding , pos.y + size_.y - expand_arrow_padding ), glm::vec2(0.f), 0xFFFFFFFF);
        tri_verts[2] = Vertex2D(glm::vec2(pos.x + size_.x - arrow_area_width + expand_arrow_padding , pos.y + size_.y - expand_arrow_padding ), glm::vec2(0.f), 0xFFFFFFFF);
    }
    canvas->BatchData(canvas->GetBlankTexture()->GetDefaultSampler(), 0, tri_verts, 3, tri_inds, 3);
    
    if (input->IsMouseUp(SM_MOUSE_LEFT) && pointer_events_)
    {
        bool prev_opened = opened_;
        opened_ = hovering;
        if (prev_opened && opened_)
            opened_ = false;
        SetFocus(opened_);
    }
        
    int mx, my;
    input->GetMousePos(&mx, &my);
    if (opened_)
    {
        int index = 0;
        float box_height = static_cast<float>(font_->GetPixelHeight()) + 15.f;
        float y_offset = expand_mode_ == EXPAND_UP ? -(box_height + 1.f) : box_height + 1.f;
        for (auto& box : options_)
        {
            bool inside = TestPoint((float)mx, (float)my,
                                    pos.x, pos.y - y_offset, size_.x, box_height) && pointer_events_;
            if (input->IsMouseDown(SM_MOUSE_LEFT) && inside)
            {
                selected_ = index;
                OnSelect(selected_);
            }
            float text_offset = input->IsMouseHeld(SM_MOUSE_LEFT) && inside && selected_ == index ? 1.f : 0.f;
            canvas->style.z_order = 2;
            canvas->style.gradient.type = Gradient::TO_TOP;
            canvas->style.gradient.a = RGBA_2_HEX(60, 60, 60, 255);
            canvas->style.gradient.b = RGBA_2_HEX(71, 71, 71, 255);
            if (inside)
            {
                canvas->style.gradient.a = RGBA_2_HEX(72, 72, 72, 255);
                canvas->style.gradient.b = RGBA_2_HEX(90, 90, 90, 255);
            }
            if (selected_ == index)
            {
                canvas->style.gradient.a = RGBA_2_HEX(40, 40, 40, 255);
                canvas->style.gradient.b = RGBA_2_HEX(50, 50, 50, 255);
            }
            canvas->AddRect(pos.x, pos.y - y_offset, size_.x, box_height);
            
            canvas->style.gradient.type = Gradient::NONE;
            canvas->style.color = RGBA_2_HEX(40, 40, 40, 255);
            canvas->AddRectOutline(pos.x, pos.y - y_offset, size_.x, box_height);
            
            if (selected_ != index)
            {
                canvas->style.color = RGBA_2_HEX(255, 255, 255, 40);
                canvas->AddLine(pos.x + 1.f           , pos.y - y_offset + box_height - 1.f, 
                                pos.x + size_.x - 1.f, pos.y - y_offset + box_height - 1.f);
            }
            
            glm::vec2 op_tcenter = font_->ComputeSize(box.first.c_str());
            canvas->style.color = selected_ == index ? 0xAFFFFFFF : 0xFFFFFFFF;
            canvas->style.z_order = 3;
            canvas->AddText(font_, pos.x + (size_.x / 2.f) - (op_tcenter.x / 2.f) + text_offset,
                                    pos.y + (box_height / 2.f) - (op_tcenter.y / 2.f) - y_offset - text_offset,
                                    box.first.c_str());
            if (inside && !box.second.empty())
                RenderTooltip(canvas, input, font_, box.second.c_str());
            
            index++;
            y_offset += expand_mode_ == EXPAND_UP ? -(box_height + 1.f) : box_height + 1.f;
        }
    }    
    canvas->ResetStyle();
    UIElement::Render(canvas, input);
}
