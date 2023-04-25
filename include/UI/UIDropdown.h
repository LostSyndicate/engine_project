//
// Created by losts_n1cs3jj on 11/6/2022.
//

#ifndef SM3_UIDROPDOWN_H
#define SM3_UIDROPDOWN_H

#include "UIElement.h"
#include <string_view>

class UIDropdown : public UIElement
{
public:
    CLASS_NAME("UIDropdown")

     typedef void (*OnSelectFn)(UIDropdown* elem, int index);

    enum ExpandMode
    {
        EXPAND_UP,
        EXPAND_DOWN,
    };
    
    static UIDropdown* Create() { return new UIDropdown(); }
    ~UIDropdown();
    
    void AddOption(const std::string_view& name);
    void AddOption(const std::string_view& name, const std::string_view& tooltip);
    void RemoveOption(int index);
    int GetOptionCount() const;
    std::string GetOption(int index) const;
    int GetSelected() const;
    void Select(int index);
    void SetExpandMode(ExpandMode mode);
    ExpandMode GetExpandMode() const;
    void OnSelectCallback(OnSelectFn fn);
    // when overriding you should also call the base function.
    virtual void OnSelect(int index);
    
    void Render(UICanvas* canvas, Input* input) override;
private:
    UIDropdown() = default;

    OnSelectFn on_select_fn_{ nullptr };
    ExpandMode expand_mode_{EXPAND_DOWN};
    bool opened_{false};
    int selected_{0};
    // pair: (text, tooltip)
    std::vector<std::pair<std::string, std::string>> options_;
};


#endif //SM3_UIDROPDOWN_H
