//
// Created by losts_n1cs3jj on 11/6/2022.
//

#ifndef SM3_UILABEL_H
#define SM3_UILABEL_H

#include "Core/Defs.h"
#include "UICanvas.h"
#include "UIElement.h"

class UILabel : public UIElement
{
public:
    CLASS_NAME("UILabel")
    static UILabel* Create() { return new UILabel(); }
    ~UILabel();

    void Render(UICanvas* canvas, Input* input) override;
private:
    UILabel() = default;
};


#endif //SM3_UILABEL_H
