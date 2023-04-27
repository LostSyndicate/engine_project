//
// Created by losts_n1cs3jj on 11/6/2022.
//

#ifndef SM3_INPUT_H
#define SM3_INPUT_H

#include <map>
#include <vector>
#include <cstring>

#include <SDL2/SDL.h>

enum SM_MOUSE 
{
    SM_MOUSE_LEFT,
    SM_MOUSE_MIDDLE,
    SM_MOUSE_RIGHT
};

enum SM_KEYMOD
{
    SM_KEYMOD_CTRL = 1 << 0,
    SM_KEYMOD_SHIFT = 1 << 1,
    SM_KEYMOD_ALT = 1 << 2
};

enum
{
    SM_INPUT_NONE = 0,
    SM_INPUT_DOWN = 1 << 0,
    SM_INPUT_HELD = 1 << 1,
    SM_INPUT_UP = 1 << 2,
};
typedef unsigned char SM_INPUT_STATE;

enum SM_CURSOR
{
    SM_CURSOR_ARROW = 0,
    SM_CURSOR_IBEAM = 1,
    SM_CURSOR_POINTER = 2
};

class Input
{
public:
    Input(SDL_Window* window);
    
    static void CreateCursors();
    static void DestroyCursors();

    bool GetInputText(char utf8_encoded_max_size_32[32]);
    unsigned int GetKeymods() const;
    bool IsKeyHeld(unsigned int sdl_key) const;
    bool IsKeyDown(unsigned int sdl_key) const;
    bool IsKeyUp(unsigned int sdl_key) const;
    bool IsMouseHeld(SM_MOUSE btn) const;
    bool IsMouseDown(SM_MOUSE btn) const;
    bool IsMouseUp(SM_MOUSE btn) const;
    bool MouseMoved() const;
    void GetMousePos(int* x, int* y);
    void SetMousePos(int x, int y);
    void GetWindowSize(int* x, int* y);
    void GetScrollWheel(int *x, int* y);
    float GetDelta() const;
    float GetElapsed() const;
    void SetCursor(SM_CURSOR cursor);
    
    void PressKey(unsigned int sdl_key);
    void ReleaseKey(unsigned int sdl_key);
    void PressMouse(SM_MOUSE button);
    void ReleaseMouse(SM_MOUSE button);
    void SetInternalMousePos(int x, int y);
    void SetInternalScrollWheel(int rx, int ry);
    void SetMouseMoved(bool value);
    void SetInputText(char utf8_32[32]);
    void BeginFrame(float delta, float elapsed);
    void HandleEvent(SDL_Event event);
    void SetSize(int width, int height);
private:
    float delta_{1.f},
          elapsed_{0.f};
    char utf8_input_[32];
    SDL_Window* window_{nullptr};
    std::map<unsigned int, SM_INPUT_STATE> key_states_;
    SM_INPUT_STATE mouse_states_[3];
    bool mouse_moved_{false};
    int mouse_x_{0},
        mouse_y_{0};
    int width_{0},
        height_{0};
    int scroll_x_{0},
        scroll_y_{0};
    unsigned int keymods_{0};
};

#endif //SM3_INPUT_H
