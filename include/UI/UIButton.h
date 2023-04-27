//
// Created by losts_n1cs3jj on 11/6/2022.
//

#ifndef SM3_UIBUTTON_H
#define SM3_UIBUTTON_H

#include "UICanvas.h"
#include "UIElement.h"

class UIButton : public UIElement
{
public:
    CLASS_NAME("UIButton")
    
    static UIButton* Create() { return new UIButton(); }
    ~UIButton();
    
    bool JustPressed() const;
    bool IsPressed() const;
    
    void Render(UICanvas* canvas, Input* input) override;
private:
    UIButton() = default;
    
    bool just_pressed_{false};
    bool pressed_{false};
};


#endif //SM3_UIBUTTON_H
