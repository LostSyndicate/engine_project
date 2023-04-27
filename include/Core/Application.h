//
// Created by losts_n1cs3jj on 11/9/2022.
//

#ifndef SM3_APPLICATION_H
#define SM3_APPLICATION_H

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/vec2.hpp>

#include "Defs.h"
#include "Input/Input.h"

#ifdef main
#	undef main
#endif

class Application
{
public:
    Application(const char* title, int width, int height);
    virtual ~Application();

    virtual void OnInit();
    virtual void OnDestroy();
    virtual void OnFrame(float delta);
    virtual void OnBeginPoll();
    virtual void OnEndPoll();
    virtual void OnEvent(SDL_Event event);
    virtual void OnResize(int width, int height);
    virtual void OnMouseMove(int x, int y, int prev_x, int prev_y);
    virtual void OnKey(SDL_KeyCode key, SM_INPUT_STATE state);
    
    void Run();
    void Stop();
    
    void SetTargetFramerate(int target_fps);
    int GetTargetFramerate() const;

    float GetDeltaTime() const;
    float GetElapsedTime() const;
    
    int GetWidth() const;
    int GetHeight() const;
    glm::vec2 GetSize() const;
    
    int GetMouseX() const;
    int GetMouseY() const;
    glm::vec2 GetMousePos() const;
    
protected:
    int mouse_x_{0};
    int mouse_y_{0};
    int width_{0};
    int height_{0};
    int target_fps_{60};
    SDL_Window* window_{nullptr};
    SDL_GLContext gl_context_{nullptr};
    bool closed_{false};
    float delta_{1.f};
    float elapsed_{0.f};
};

#endif //SM3_APPLICATION_H
