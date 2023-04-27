//
// Created by losts_n1cs3jj on 11/2/2022.
//

#include "Graphics/Context.h"

#include <glad/glad.h>

static Context* s_Instance{nullptr};

Context *Context::Create(SDL_Window *window, SDL_GLContext gl_context)
{
    if (s_Instance != nullptr)
        return nullptr;
    Context* context = new Context();
    context->window_ = window;
    context->gl_context_ = gl_context;
    s_Instance = context;
    return context;
}

Context *Context::GetInstance()
{
    return s_Instance;
}

void Context::Viewport(int x, int y, int w, int h)
{
    glViewport(x, y, (GLsizei)w, (GLsizei)h);
}

void Context::Clear(SM_CLEAR_TYPE type, glm::vec4 color)
{
    GLenum gl_cl_type = GL_COLOR_BUFFER_BIT;
    if (type & SM_CLEAR_DEPTH)
        gl_cl_type = GL_DEPTH_BUFFER_BIT;
    glClearColor(color.x, color.y, color.z, color.w);
    glClear(gl_cl_type);
}

void Context::EnableDepth()
{
    glEnable(GL_DEPTH_TEST);
}

void Context::DisableDepth()
{
    glDisable(GL_DEPTH_TEST);
}