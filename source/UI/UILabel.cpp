//
// Created by losts_n1cs3jj on 11/6/2022.
//

#include "UI/UILabel.h"

UILabel::~UILabel() {}

void UILabel::Render(UICanvas* canvas, Input* input)
{
    glm::vec2 pos = GetVisiblePosition();
    canvas->style.z_order = 1;
    canvas->AddText(font_, pos.x, pos.y, text_.c_str());
    canvas->ResetStyle();
    UIElement::Render(canvas, input);
}