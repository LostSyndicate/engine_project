//
// Created by losts_n1cs3jj on 11/6/2022.
//

#include "Input/Input.h"
#include "SDL2/SDL_keyboard.h"

static SDL_Cursor* sys_cursors_[4] = {
    nullptr, nullptr, nullptr, nullptr
};

Input::Input(SDL_Window* window)
{
    window_ = window;
}

void Input::CreateCursors()
{
    sys_cursors_[0] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    sys_cursors_[1] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
    sys_cursors_[2] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
}

void Input::DestroyCursors()
{
    for (int i = 0; i < 4; i++) {
        if (sys_cursors_[i] != nullptr)
            SDL_FreeCursor(sys_cursors_[i]);
    }
}

bool Input::GetInputText(char* utf8_encoded_max_size_32)
{
    memcpy(utf8_encoded_max_size_32, utf8_input_, 32);
    return utf8_input_[0] != 0;
}

unsigned int Input::GetKeymods() const
{
    return keymods_;
}

bool Input::IsKeyHeld(unsigned int key) const
{
    auto f = key_states_.find(key);
    return !(f == key_states_.end()) && (f->second & SM_INPUT_HELD);
}

bool Input::IsKeyDown(unsigned int key) const
{
    auto f = key_states_.find(key);
    return !(f == key_states_.end()) && (f->second & SM_INPUT_DOWN);
}

bool Input::IsKeyUp(unsigned int key) const
{
    auto f = key_states_.find(key);
    return !(f == key_states_.end()) && (f->second & SM_INPUT_UP);
}

bool Input::IsMouseHeld(SM_MOUSE btn) const
{
    return mouse_states_[btn] & SM_INPUT_HELD;
}

bool Input::IsMouseDown(SM_MOUSE btn) const
{
    return mouse_states_[btn] & SM_INPUT_DOWN;
}

bool Input::IsMouseUp(SM_MOUSE btn) const
{
    return mouse_states_[btn] & SM_INPUT_UP;
}

bool Input::MouseMoved() const
{
    return mouse_moved_;
}

void Input::GetMousePos(int *x, int *y)
{
    if (x != nullptr) *x = mouse_x_;
    if (y != nullptr) *y = mouse_y_;
}

void Input::SetMousePos(int x, int y)
{
    // Window polling causes MOUSE_MOVE event, it is not necessary to set mouse_x/mouse_y.
    SDL_WarpMouseInWindow(window_, x, y);
}

void Input::GetWindowSize(int* x, int* y)
{
    if (x != nullptr) *x = width_;
    if (y != nullptr) *y = height_;
}

void Input::GetScrollWheel(int *x, int* y)
{
    if (x != nullptr) *x = scroll_x_;
    if (y != nullptr) *y = scroll_y_;
}

float Input::GetDelta() const
{
    return delta_;
}

float Input::GetElapsed() const
{
    return elapsed_;
}

void Input::SetCursor(SM_CURSOR cursor)
{
    SDL_SetCursor(sys_cursors_[cursor]);
}

void Input::PressKey(unsigned int key)
{
    auto f = key_states_.find(key);
    if (f != key_states_.end())
        f->second = SM_INPUT_DOWN | SM_INPUT_HELD;
    else
        key_states_.emplace(key, SM_INPUT_DOWN | SM_INPUT_HELD);
}

void Input::ReleaseKey(unsigned int key)
{
    auto f = key_states_.find(key);
    if (f != key_states_.end())
        f->second = SM_INPUT_UP;
    else
        key_states_.emplace(key, SM_INPUT_UP);
}

void Input::PressMouse(SM_MOUSE button)
{
    mouse_states_[button] = SM_INPUT_DOWN | SM_INPUT_HELD;
}

void Input::ReleaseMouse(SM_MOUSE button)
{
    mouse_states_[button] = SM_INPUT_UP;
}

void Input::SetInternalMousePos(int x, int y)
{
    mouse_x_ = x;
    mouse_y_ = height_ - y;
}

void Input::SetInternalScrollWheel(int rx, int ry)
{
    scroll_x_ = rx;
    scroll_y_ = ry;
}

void Input::SetMouseMoved(bool value)
{
    mouse_moved_ = value;
}

void Input::SetInputText(char utf8_32[32])
{
    memcpy(utf8_input_, utf8_32, 32);
}

void Input::BeginFrame(float delta, float elapsed)
{
    // Clear all flags except held, since held spans until the key is released.
    for (auto& key : key_states_)
        key.second &= SM_INPUT_HELD;
    // filter out keys that are not pressed.
    std::map<unsigned int, SM_INPUT_STATE> replacement;
    for (auto& key : key_states_)
    {
        if (key.second)
            replacement.emplace(key.first, key.second);
    }
    key_states_ = replacement;
    // clear flags besides held
    mouse_states_[0] &= SM_INPUT_HELD;
    mouse_states_[1] &= SM_INPUT_HELD;
    mouse_states_[2] &= SM_INPUT_HELD;
    utf8_input_[0] = 0;
    scroll_x_ = 0;
    scroll_y_ = 0;
    mouse_moved_ = false;
    delta_ = delta;
    elapsed_ = elapsed;
    keymods_ = 0;
    SDL_Keymod mods = SDL_GetModState();
    if (mods & KMOD_SHIFT) keymods_ |= SM_KEYMOD_SHIFT;
    if (mods & KMOD_CTRL) keymods_ |= SM_KEYMOD_CTRL;
    if (mods & KMOD_ALT) keymods_ |= SM_KEYMOD_ALT;
}

void Input::HandleEvent(SDL_Event event)
{
    SM_MOUSE mb = SM_MOUSE_LEFT;
    switch (event.type)
    {
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                SetSize((int)event.window.data1, (int)event.window.data2);
            break;
        case SDL_TEXTINPUT:
            SetInputText(event.text.text);
            break;
        case SDL_MOUSEWHEEL:
            SetInternalScrollWheel(event.wheel.x, event.wheel.y);
            break;
        case SDL_MOUSEBUTTONDOWN:
            switch (event.button.button)
            {
                case SDL_BUTTON_LEFT: mb = SM_MOUSE_LEFT; break;
                case SDL_BUTTON_MIDDLE: mb = SM_MOUSE_MIDDLE; break;
                case SDL_BUTTON_RIGHT: mb = SM_MOUSE_RIGHT; break;
                default: break;
            }
            PressMouse(mb);
            break;
        case SDL_MOUSEBUTTONUP:
            switch (event.button.button)
            {
                case SDL_BUTTON_LEFT: mb = SM_MOUSE_LEFT; break;
                case SDL_BUTTON_MIDDLE: mb = SM_MOUSE_MIDDLE; break;
                case SDL_BUTTON_RIGHT: mb = SM_MOUSE_RIGHT; break;
                default: break;
            }
            ReleaseMouse(mb);
            break;
        case SDL_KEYDOWN:
            PressKey(event.key.keysym.sym);
            break;
        case SDL_KEYUP:
            ReleaseKey(event.key.keysym.sym);
            break;
        case SDL_MOUSEMOTION:
            SetMouseMoved(true);
            SetInternalMousePos(event.motion.x, event.motion.y);
            break;
        default:
            break;
    }
}

void Input::SetSize(int width, int height)
{
    width_ = width;
    height_ = height;
}