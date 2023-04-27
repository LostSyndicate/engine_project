//
// Created by losts_n1cs3jj on 11/16/2022.
//

#include "UI/UIField.h"
#include "Core/Util.h"

UIField::~UIField() {}

void UIField::SetPlaceholder(const std::string_view& view)
{
    placeholder_ = view;
}

std::string UIField::GetPlaceholder() const
{
    return placeholder_;
}

void UIField::SetFieldType(FieldType type)
{
    field_type_ = type;
}

UIField::FieldType UIField::GetFieldType() const
{
    return field_type_;
}

void UIField::SetStepInterval(float interval)
{
    step_interval_ = interval;
}

float UIField::GetStepInterval() const
{
    return step_interval_;
}

void UIField::SetNumber(float* value)
{
    switch (field_type_)
    {
        case NUMBER:
            number_value_[0] = *value;
            break;
        case VEC2: 
            number_value_[0] = value[0];
            number_value_[1] = value[1];
            break;
        case VEC3: 
            number_value_[0] = value[0]; 
            number_value_[1] = value[1];
            number_value_[2] = value[2];
            break;
        case VEC4: 
            number_value_[0] = value[0];
            number_value_[1] = value[1];
            number_value_[2] = value[2];
            number_value_[3] = value[3];
            break;
        default:
            break;
    }
}

void UIField::GetNumber(float* value) const
{
    switch (field_type_)
    {
        case NUMBER:
            value[0] = number_value_[0];
            break;
        case VEC2: 
            value[0] = number_value_[0];
            value[1] = number_value_[1];
            break;
        case VEC3: 
            value[0] = number_value_[0]; 
            value[1] = number_value_[1];
            value[2] = number_value_[2];
            break;
        case VEC4: 
            value[0] = number_value_[0];
            value[1] = number_value_[1];
            value[2] = number_value_[2];
            value[3] = number_value_[3];
            break;
        default:
            break;
    }
}

bool UIField::IsTextValid(char text[32])
{
    if (field_type_ == TEXT)
        return true;
    else
    {
        unsigned int code = 0;
        smGetUTF8CharPoint(text, 0, &code);
        return (code >= '0' && code <= '9') || (code == '.' && text_.find_first_of('.') == std::string::npos);
    }
}

void UIField::Render(UICanvas* canvas, Input* input)
{
    glm::vec2 pos = GetVisiblePosition();
    glm::vec2 text_size = font_->ComputeSize(text_.empty() ? placeholder_.c_str() : text_.c_str());
    bool hovering = IsMouseHovered(input) && pointer_events_;
    
    canvas->style.color = RGBA_2_HEX(46, 46, 46, 255);
    canvas->AddRect(pos.x - 1.f, pos.y - 1.f, size_.x + 2.f, size_.y + 2.f);
    
    canvas->style.gradient.type = Gradient::TO_TOP;
    canvas->style.gradient.a = RGBA_2_HEX(54, 54, 54, 255);
    canvas->style.gradient.b = RGBA_2_HEX(40, 40, 40, 255);
    if (hovering)
    {
        canvas->style.gradient.a = RGBA_2_HEX(54, 54, 54, 255);
        canvas->style.gradient.b = RGBA_2_HEX(60, 60, 60, 255);
        if (input->IsMouseUp(SM_MOUSE_LEFT))
            SetFocus(true);
    }
    if (!hovering && input->IsMouseDown(SM_MOUSE_LEFT))
        SetFocus(false);
        
    if (hovering && !last_hovering_)
        input->SetCursor(SM_CURSOR_IBEAM);
    else if (!hovering && last_hovering_)
        input->SetCursor(SM_CURSOR_ARROW);

    last_hovering_ = hovering;
    canvas->AddRect(pos.x, pos.y, size_.x, size_.y);

    blink_time_ += input->GetDelta();
    blink_time_ = fmodf(blink_time_, 1.f);
    char input_text[32];
    if (IsFocused())
    {
        unsigned int mods = input->GetKeymods();

        if (input->IsKeyDown(SDLK_LEFT))
        {
            cursor_pos_ = cursor_pos_ == -1 ? smUTF8CharCount(text_.c_str(), 0) - 1 : cursor_pos_ - 1;
            blink_time_ = 0.f;
            if (cursor_pos_ < 0) cursor_pos_ = 0;
        }
        if (input->IsKeyDown(SDLK_RIGHT))
        {
            if (cursor_pos_ != -1)
                cursor_pos_++;
            blink_time_ = 0.f;
            if (cursor_pos_ >= smUTF8CharCount(text_.c_str(), 0))
                cursor_pos_ = -1;
        }
        
        if (blink_time_ < 0.5f || input->GetElapsed() - last_input_time_ < 0.5)
        {
            float tx = 0.f;
            float ty = text_size.y;
            if (!text_.empty())
                tx = cursor_pos_ != -1 ? font_->ComputeSize(text_.c_str(), cursor_pos_).x : text_size.x;
            canvas->style.gradient.type = Gradient::NONE;
            canvas->style.color = 0xFFFFFFFF;
            canvas->AddRect(pos.x + tx + 11.f, pos.y + (size_.y / 2.f) - (ty / 2.f), 1.f, ty);
        }
                
        if (input->IsKeyDown(SDLK_BACKSPACE) && !text_.empty())
        {
            if (mods & SM_KEYMOD_CTRL)
            {
                // this is so horribly ineffecient please fix it..
                bool whtspc = false;
                std::vector<std::pair<unsigned int, unsigned int>> codes;
                int index = 0;
                while (index < text_.size())
                {
                    unsigned int code;
                    int count = smGetUTF8CharPoint(text_.c_str(), index, &code);
                    codes.push_back(std::pair<unsigned int, unsigned int>(code, index));
                    index += count;
                }
                int end_index = 0;
                for (auto i = codes.size(); i-- > 0;) {
                    if (i == codes.size() - 1) {
                        if (codes[i].first == 32 || codes[i].first == 9) // space(32) tab(9)
                            whtspc = true;
                    }
                    else {
                        if (whtspc && codes[i].first != 32 && codes[i].first != 9) {
                            end_index = codes[i].second;
                            break;
                        }
                        else if (!whtspc && (codes[i].first == 32 || codes[i].first == 9)) {
                            end_index = codes[i].second ;
                            break;
                        }
                    }
                }
                text_ = text_.substr(0, end_index != 0 ? end_index + 1 : end_index);
            }
            else
            {
                // TODO: This doesnt work all the time...
                int ch_size = 1;
                int index = 0;
                if (cursor_pos_ == -1)
                    index = smUTF8LastCharIndex(text_.c_str(), 0, &ch_size);
                else
                {
                    int ch_count = smUTF8CharCount(text_.c_str(), 0);
                    index = smUTF8AtPos(text_.c_str(), 0,  (ch_count - (ch_count - cursor_pos_)) - 1, &ch_size);
                    if (cursor_pos_ == 0)
                        ch_size = 0;
                    cursor_pos_--;
                    if (cursor_pos_ < 0)
                        cursor_pos_ = 0;
                }
                if (ch_size > 0)
                    text_.erase(index, ch_size);
            }
            if (cursor_pos_ >= smUTF8CharCount(text_.c_str(), 0))
                cursor_pos_ = -1;
            last_input_time_ = input->GetElapsed();
            blink_time_ = 0.f;
        }
        
        if (input->GetInputText(input_text) && text_.size() < 256 && IsTextValid(input_text))
        {
            unsigned int code;
            int count = smGetUTF8CharPoint(input_text, 0, &code);
            int index = text_.size();
            if (cursor_pos_ != -1) {
                index = smUTF8AtPos(text_.c_str(), 0, cursor_pos_, NULL);
                cursor_pos_++;
            }
            for  (int i = 0; i < count; i++) {
                text_.insert(text_.begin() + index, input_text[(count - 1) - i]);
            }
            last_input_time_ = input->GetElapsed();
            blink_time_ = 0.f;
        }
    }
    
    canvas->style.gradient.type = Gradient::NONE;
    canvas->style.color = text_.empty() ? 0x7FFFFFFF : 0xFFFFFFFF;
    canvas->style.z_order = 1;
    canvas->AddText(font_, pos.x + 10.f, pos.y + (size_.y / 2.f) - (text_size.y / 2.f), text_.empty() ? placeholder_.c_str() : text_.c_str());
    canvas->ResetStyle();
    
    UIElement::Render(canvas, input);
}