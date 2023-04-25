//
// Created by losts_n1cs3jj on 11/6/2022.
//

#ifndef SM3_UIWINDOW_H
#define SM3_UIWINDOW_H

#include "UIElement.h"

class UIWindow : public UIElement
{
public:
    CLASS_NAME("UIWindow")
    
    static UIWindow* Create() { return new UIWindow(); }
    ~UIWindow();
    
    void AddToView(UIElement* element);
    void RemoveFromView(UIElement* element);
    
    void Render(UICanvas* canvas, Input* input) override;
private:
    UIWindow() = default;
    
    bool is_grabbing_scrollbar_x_{false};
    bool is_grabbing_scrollbar_y_{false};
    glm::vec2 scrollbar_grab_offset_{0.f};
    glm::vec2 scrollbar_offset_{0.f};
    std::vector<UIElement*> elements_;
};


#endif //SM3_UIWINDOW_H
