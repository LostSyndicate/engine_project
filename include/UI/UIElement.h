//
// Created by losts_n1cs3jj on 11/6/2022.
//

#ifndef SM3_UIELEMENT_H
#define SM3_UIELEMENT_H

#include <string>
#include <cstdint>
#include <glm/vec2.hpp>

#include "Core/Reference.h"
#include "Input/Input.h"
#include "UICanvas.h"

#define RGBA_2_HEX(r,g,b,a) (uint32_t)( (a << 24) | (b << 16) | (g << 8) | r)

// All UI elements should always start at the bottom-left of the screen.
class UIElement : public Reference
{
    friend class UIWindow;
public:
    CLASS_NAME("UIElement")
    
    // controls where an element should be 'anchored'.
    // for example, if you wanted to align some element to the 'middle', you would either write (0.5, 0.5) or use 'UIAnchorPos::Middle' then when 
    // the element is at position (0,0), it would be rendered in the middle of whatever it is parented to.
    // this enum is just an alternative for writing the anchor positions, and is not required to be used.
    enum Anchor
    {
        // anchor values (note: this naming is only accurate if 0,0 is bottom-left.)
        A_LEFT = 1 << 0,                                // (0.0, 0.5)
        A_RIGHT = 1 << 1,                               // (1.0, 0.5)
        A_TOP = 1 << 2,                                 // (0.5, 1.0)
        A_BOTTOM = 1 << 3,                              // (0.5, 0.0)
        A_TOPLEFT = A_TOP | A_LEFT,                     // (0.0, 1.0)
        A_TOPRIGHT = A_TOP | A_RIGHT,                   // (1.0, 1.0)
        A_BOTTOMLEFT = A_BOTTOM | A_LEFT,               // (0.0, 0.0), default.
        A_BOTTOMRIGHT = A_BOTTOM | A_RIGHT,             // (1.0, 0.0)
        A_MIDDLE = A_LEFT | A_RIGHT | A_TOP | A_BOTTOM, // (0.5, 0.5)
        A_DEFAULT = A_BOTTOMLEFT,                       // default is always assigned to (0,0 | or BottomLeft), which should always be the bottom-left.
    };

    static UIElement* GetFocused();
    static bool TestPoint(int px, int py,
                          float rx, float ry, float rw, float rh);
    static void RenderTooltip(UICanvas* canvas, Input* input, Font* font, const char* tooltip);
    static glm::vec2 AnchorToValue(Anchor anchor);

    ~UIElement();
    
    /**
     * Allows you to specify where '0,0' starts, relative to the parents dimensions.
     * For example, if you wanted 0,0 to be at the top-right, you would specify (1.0, 1.0) or UIAnchorPos::TopRight.
     * 
     * 
     * TODO: Is below really necessary to do? This would just be more confusing then helpful, unless its used specifically at 1.0,0.5,and 0.0
     * 
     * Below is a diagram for the axes.
     *     
     * Example: DR -,+ (Direction, X,Y signs, in this case at (3,2) is 3 to the left and 2 up.).
     *     
     *     TL (+,-)  |  (-,-) TR 
     *               |
     *          (middle +,+)
     *     ----------+----------
     *               |
     *               |
     *     BL (+,+)  |  (-,+) BR
     * 
     *
     */
    void SetAnchor(glm::vec2 anchor);
    glm::vec2 GetAnchor() const;

    /**
     * Aligns the content to where the anchor is placed.
     * For example, if you had an image and wanted it to be at the middle instead of the bottom-left, you would simply specify a percentage (0.5, 0.5),
     *   or use UIAnchorPos::Middle.
     * Note: All descendants will be offset to wherever the bottom-left of the content is.
     *       Inspect the diagram below for more detail.
     * 
     * ----------------------------
     * 
     * (position | 0,0)
     * |___
     * V   | <- (content)
     * .___|
     * 
     * align to: (0.5, 0.5)
     * 
     * (element position is still at 0,0)
     *  ___
     * | . | (but content is now aligned to middle)
     * |___|
     * ^
     * | (this is where the 'bottom-left' of the content is, and all descendents will be offset to here).
     */
    void SetAlignment(glm::vec2 anchor);
    glm::vec2 GetAlignment() const;

    // For elements that contain text, use this to align the text.
    void SetTextAlignment(Anchor pos);
    Anchor GetTextAlignment() const;

    void SetPosition(const glm::vec2& position);
    glm::vec2 GetPosition() const;
    // get the visible position (ie when scrolling this will change)
    glm::vec2 GetVisiblePosition() const;
    void SetSize(const glm::vec2& size);
    glm::vec2 GetSize() const;
    void SetFont(Font* font);
    Font* GetFont() const;
    void SetText(const std::string_view& text);
    std::string GetText() const;
    bool IsFocused() const;
    void SetFocus(bool focus);
    void SetDefaultSize();
    void SetTooltip(const std::string_view& tooltip);
    std::string GetTooltip() const;
    void EnablePointerEvents(bool enabled);
    bool IsPointerEventsEnabled() const;
    void SetEnabled(bool value);
    bool IsEnabled() const;
    void SetData(void* data);
    void* GetData() const;

    virtual void Render(UICanvas* canvas, Input* input);
    
protected:
    UIElement() = default;
    bool IsMouseHovered(Input* input) const;
    glm::vec2 CalcTextOffset(const char* text);

    bool enabled_{true};
    bool pointer_events_{true};
    std::string tooltip_;
    std::string text_;
    Font* font_{nullptr};
    glm::vec2 position_{0.f};
    glm::vec2 offset_{0.f};
    glm::vec2 content_offset_{0.f};
    glm::vec2 size_{0.f};
    glm::vec2 anchor_{0.f};
    glm::vec2 alignment_{0.f};
    void* data_{ nullptr };
    Anchor text_align_{A_MIDDLE};
};


#endif //SM3_UIELEMENT_H
