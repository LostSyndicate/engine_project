//
// Created by losts_n1cs3jj on 11/9/2022.
//

#include "Core/Application.h"

Application::Application(const char* title, int width, int height)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, SM_DEPTH_SIZE);
    window_ = SDL_CreateWindow(title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    gl_context_ = SDL_GL_CreateContext(window_);
    gladLoadGL();
    width_ = width;
    height_ = height;
}

Application::~Application()
{
    SDL_GL_DeleteContext(gl_context_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

void Application::OnInit() {}
void Application::OnDestroy() {}
void Application::OnFrame(float delta) {}
void Application::OnBeginPoll() {}
void Application::OnEndPoll() {}
void Application::OnEvent(SDL_Event event) {}
void Application::OnResize(int width, int height) {}
void Application::OnMouseMove(int x, int y, int prev_x, int prev_y) {}
void Application::OnKey(SDL_KeyCode key, SM_INPUT_STATE state) {}

void Application::Run()
{
    delta_ = 1.f;
    closed_ = false;
    while (!closed_)
    {
        Uint32 t_ms = static_cast<Uint32>((1.f / target_fps_) * 1000.f);
        Uint64 start = SDL_GetTicks64();
        elapsed_ = static_cast<float>(SDL_GetTicks64() / 1000.0);
        SDL_Event event;
        OnBeginPoll();
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                    {
                        width_ = (int)event.window.data1;
                        height_ = (int)event.window.data2;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    mouse_x_ = (int)event.motion.x;
                    mouse_y_ = (int)event.motion.y;
                    break;
                case SDL_QUIT:
                    closed_ = true;
                    break;
            }
            OnEvent(event);
        }
        OnEndPoll();
        OnFrame(delta_);
        Uint64 delay = SDL_GetTicks64() - start;
        if (delay < t_ms)
            SDL_Delay(t_ms - delay);
        delta_ = (float)(SDL_GetTicks64() - start) / 1000.f;
        SDL_GL_SwapWindow(window_);
    }
}

void Application::Stop()
{
    closed_ = true;
}

void Application::SetTargetFramerate(int target_fps)
{
    target_fps_ = target_fps;
}

int Application::GetTargetFramerate() const
{
    return target_fps_;
}

float Application::GetDeltaTime() const
{
    return delta_;
}

float Application::GetElapsedTime() const
{
    return elapsed_;
}

int Application::GetWidth() const
{
    return width_;
}

int Application::GetHeight() const
{
    return height_;
}

glm::vec2 Application::GetSize() const
{
    return glm::vec2(width_, height_);
}

int Application::GetMouseX() const
{
    return mouse_x_;
}

int Application::GetMouseY() const
{
    return mouse_y_;
}

glm::vec2 Application::GetMousePos() const
{
    return glm::vec2(mouse_x_, mouse_y_);
}