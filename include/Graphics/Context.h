//
// Created by losts_n1cs3jj on 11/2/2022.
//

#ifndef SM3_CONTEXT_H
#define SM3_CONTEXT_H

#include <SDL2/SDL.h>
#include <glm/vec4.hpp>

enum SM_CLEAR_TYPE
{
    SM_CLEAR_COLOR,
    SM_CLEAR_DEPTH
};

class Context
{
public:
    static Context* Create(SDL_Window* window, SDL_GLContext gl_context);
    static Context* GetInstance();
    
    /// Setup viewport.
    /// \param x X position.
    /// \param y Y position.
    /// \param w Width of the viewport.
    /// \param h Height of the viewport.
    void Viewport(int x, int y, int w, int h);
    
    /// Clears the specified bits.
    /// \param type The type of bits to clear.
    /// \param color The color to clear with. Default is black.
    void Clear(SM_CLEAR_TYPE type, glm::vec4 color = glm::vec4(0.f, 0.f, 0.f, 1.f));
    
    /// Enable depth testing.
    void EnableDepth();
    
    /// Disable depth testing.
    void DisableDepth();
private:
    Context() = default;
    
    SDL_Window* window_{nullptr};
    SDL_GLContext gl_context_{nullptr};
};


#endif //SM3_CONTEXT_H
