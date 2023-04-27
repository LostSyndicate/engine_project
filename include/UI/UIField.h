//
// Created by losts_n1cs3jj on 11/16/2022.
//

#ifndef SM3_UIFIELD_H
#define SM3_UIFIELD_H

#include "UIElement.h"

class UIField : public UIElement
{
public:
    CLASS_NAME("UIField")
    
    enum FieldType
    {
        TEXT,
        NUMBER,
        VEC2,
        VEC3,
        VEC4,
    };
    
    static UIField* Create() { return new UIField(); }
    ~UIField();
    
    void SetPlaceholder(const std::string_view& view);
    std::string GetPlaceholder() const;
    
    void SetFieldType(FieldType type);
    FieldType GetFieldType() const;
    
    void SetStepInterval(float interval);
    float GetStepInterval() const;
    void SetNumber(float* value);
    void GetNumber(float* value) const;
    
    void Render(UICanvas* canvas, Input* input) override;
private:
    UIField() = default;
    
    bool IsTextValid(char text[32]);
    
    FieldType field_type_{TEXT};
    float number_value_[4];
    float step_interval_{0.1f};
    int cursor_pos_{-1};
    float blink_time_{0.f};
    float last_input_time_{0.f};
    bool last_hovering_{false};
    std::string placeholder_ = "enter text...";
};


#endif //SM3_UIFIELD_H
