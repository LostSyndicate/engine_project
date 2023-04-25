#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/gtc/quaternion.hpp>

#include "Graphics/Shader.h"
#include "Graphics/Mesh.h"
#include "Graphics/Camera.h"
#include "Graphics/Texture.h"
#include "Graphics/Skybox.h"
#include "Graphics/Model.h"
#include "Graphics/Framebuffer.h"
#include <stb/stb_image.h>
#include <glm/gtx/quaternion.hpp>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#ifdef main
#   undef main
#endif

#define SHADOW_WIDTH 4096
#define SHADOW_HEIGHT 4096

Skybox* skybox_ = nullptr;
Texture* texture_ = nullptr;
Shader* shader_ = nullptr;
Shader* shader2_ = nullptr;
Shader* shader3_ = nullptr;
Shader* shader4_ = nullptr;
Shader* skybox_shader_ = nullptr;
Mesh* mesh_ = nullptr;
Mesh* mesh2_ = nullptr;
Mesh* mesh3_ = nullptr;
Model* model_ = nullptr;
Framebuffer* sm_fb_ = nullptr;

float lx = 6.f, ly = 25.f, lz = -3.f;

Camera camera = Camera();

void export_cubemap(int size, int comp, int x, int y, const char* name, const unsigned char* pixels)
{
    unsigned char* region = (unsigned char*)malloc( (size * size) * comp);
    int xs = x * size;
    int ys = y * size;
    for (int ry = 0; ry < size; ry++)
    {
        for (int rx = 0; rx < size; rx++)
        {
            for (int c = 0; c < comp; c++)
                region[(rx + ry * size) * comp + c] = pixels[
            (rx + ry * size) * comp + c];
                        //((xs + rx + ((ys + ry) * size)) * comp) + c];
        }
    }
    stbi_write_jpg(name, size, size, comp, (const void*)region, 0);
    free(region);
}

void draw_scene(float delta, Shader* shader, bool light_pass)
{
    if (shader->IsCompiled())
    {
        shader->Bind();
        if (!light_pass)
        {
            texture_->Bind(0);
            sm_fb_->GetAttachment(0)->Bind(1);
        }
     
        if (!light_pass)
        {
            shader->GetUniform("ambientStrength")->SetValue(0.25f);
            shader->GetUniform("lightPos")->SetValue(glm::vec3(lx, ly, lz));
            shader->GetUniform("viewPos")->SetValue(camera.GetPosition());
            shader->GetUniform("view")->SetValue(camera.GetViewMatrix());
            shader->GetUniform("projection")->SetValue(camera.GetProjMatrix());
        }
        
        shader->GetUniform("model")->SetValue(glm::scale(glm::vec3(0.01f, 0.01f, 0.01f)));
        model_->Draw();
        
        mesh_->SetPosition(glm::vec3(0.f, -1.f, 0.f));
        mesh_->SetScale(glm::vec3(100.f, 0.1f, 100.f));
        shader->GetUniform("model")->SetValue(mesh_->GetModelMatrix());
        mesh_->Draw();
    }
}

int main(int argc, char* argv[])
{
//    int cx, cy, ch;
//    stbi_uc* px = stbi_load("day.png", &cx, &cy, &ch, 0);
//    export_cubemap(2024, ch, 0, 0, "bottom.jpg", px);
//    export_cubemap(2024, ch, 1, 0, "top.jpg", px);
//    export_cubemap(2024, ch, 2, 0, "back.jpg", px);
//    export_cubemap(2024, ch, 0, 1, "left.jpg", px);
//    export_cubemap(2024, ch, 1, 1, "front.jpg", px);
//    export_cubemap(2024, ch, 2, 1, "right.jpg", px);
//    stbi_image_free(px);

// TODO: SSAO...!!!!
    
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, SM_DEPTH_SIZE);
    SDL_Window* window = SDL_CreateWindow("Window",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    gladLoadGL();
    shader_ = Shader::Create();
    shader2_ = Shader::Create();
    shader3_ = Shader::Create();
    shader4_ = Shader::Create();
    skybox_shader_ = Shader::Create();
    shader_->CompileFile("res/shaders/shaded.vert.glsl", "res/shaders/shaded.frag.glsl");
    shader2_->CompileFile("res/shaders/world_line.vert.glsl", "res/shaders/world_line.frag.glsl");
    shader3_->CompileFile("res/shaders/depth.vert.glsl", "res/shaders/depth.frag.glsl");
    shader4_->CompileFile("res/shaders/textured.vert.glsl", "res/shaders/textured.frag.glsl");
    skybox_shader_->CompileFile("res/shaders/cubemap.vert.glsl", "res/shaders/cubemap.frag.glsl");
    skybox_ = Skybox::Create();
    skybox_->Load({
          "res/skybox2/right.png",
          "res/skybox2/left.png",
          "res/skybox2/top.png",
          "res/skybox2/bottom.png",
          "res/skybox2/front.png",
          "res/skybox2/back.png"
    });
    mesh_ = Mesh::Create(SM_MESH_CUBE);
    mesh2_ = Mesh::Create();
    Vertex lverts[] = {
            Vertex(glm::vec3(0, 0, 0), glm::vec3(0), glm::vec2(0)),
            Vertex(glm::vec3(0, 0, 1000), glm::vec3(0), glm::vec2(0)),
            Vertex(glm::vec3(0, 1000, 0), glm::vec3(0), glm::vec2(0)),
            Vertex(glm::vec3(1000, 0, 0), glm::vec3(0), glm::vec2(0))
    };
    unsigned short linds[] = {0,1,0,2,0,3};
    mesh2_->SetVertices(lverts, 4);
    mesh2_->SetIndices(linds, 6);
    mesh3_ = Mesh::Create();
    Vertex qverts[] = {
            Vertex(glm::vec3(-1.f, -1.f, 0.f), glm::vec3(0.f), glm::vec2(0.f, 0.f)),
            Vertex(glm::vec3(1.f, -1.f, 0.f), glm::vec3(0.f), glm::vec2(1.f, 0.f)),
            Vertex(glm::vec3(1.f, 1.f, 0.f), glm::vec3(0.f), glm::vec2(1.f, 1.f)),
            Vertex(glm::vec3(-1.f, 1.f, 0.f), glm::vec3(0.f), glm::vec2(0.f, 1.f))

//            Vertex(glm::vec3(-1.f, -1.f, 0.f), glm::vec3(0.f), glm::vec2(0.f, 0.f)),
//            Vertex(glm::vec3(-0.5f, -1.f, 0.f), glm::vec3(0.f), glm::vec2(1.f, 0.f)),
//            Vertex(glm::vec3(-0.5f, -0.5f, 0.f), glm::vec3(0.f), glm::vec2(1.f, 1.f)),
//            Vertex(glm::vec3(-1.f, -0.5f, 0.f), glm::vec3(0.f), glm::vec2(0.f, 1.f))
    };
    unsigned short qinds[] = {0,1,3,1,2,3};
    mesh3_->SetVertices(qverts, 4);
    mesh3_->SetIndices(qinds, 6);
    texture_ = Texture::Create();
    texture_->Load("res/textures/wood_planks.jpg");
    model_ = Model::Create();
    model_->Load("res/Sponza/sponza.obj");
    mesh_->SetSampler(texture_->GetDefaultSampler());
    sm_fb_ = Framebuffer::Create(SHADOW_WIDTH, SHADOW_HEIGHT);
    sm_fb_->CreateAttachment(SM_ATTACHMENT_TYPE_DEPTH);
    
    ///////////////////////////////////////////////////////

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);
    
    glEnable(GL_DEPTH_TEST);
    
    ///////////////////////////////////////////////////////
    
    int w=1280, h=720;
    
    glViewport(0, 0, w, h);
    
    float delta = 1.f;
    Uint64 t_ms = 16;
    bool closed = false,
         captured = true;
    while (!closed)
    {
        Uint64 t_start = SDL_GetTicks64();
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_WINDOWEVENT:
                {
                    if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                        w = (int) event.window.data1;
                        h = (int) event.window.data2;
                        camera.SetDimensions((float)w, (float)h);
                    }
                    break;
                }
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        captured = !captured;
                        if (!captured)
                            SDL_ShowCursor(SDL_ENABLE);
                    }
                    camera.OnKeyPress((unsigned int)event.key.keysym.sym);
                    break;
                case SDL_KEYUP:
                    camera.OnKeyRelease((unsigned int)event.key.keysym.sym);
                    break;
                case SDL_MOUSEMOTION:
                    if (captured)
                        camera.OnMouseMove(event.motion.xrel, event.motion.yrel);
                    break;
                case SDL_QUIT:
                    closed = true;
                    break;
            }
        }
    
        if (captured)
        {
            int mx, my;
            SDL_GetMouseState(&mx, &my);
            if (mx != w/2 || my != h/2)
            {
                SDL_ShowCursor(SDL_DISABLE);
                SDL_WarpMouseInWindow(window, w/2, h/2);
            }
        }
    
        camera.OnUpdate(delta);
    
        char window_title[64];
        snprintf(window_title, sizeof(window_title), "Window | %.1f FPS", (1.0f / delta));
        SDL_SetWindowTitle(window, window_title);
        
        static bool dir = true;
        static float accum = 0.f;
        accum += delta;

        if (lz >= 20.f || lz <= -20.f)
            dir = !dir;
        lz += dir ? -(delta * 0.1f) : delta * 0.1f;
        lz = std::clamp(lz, -20.f, 20.f);
        
        // light quaternion (facing angled Z axis)
        // 0, 0, -0.5, 0.866
        float near_plane = 1.f, far_plane = 45.f;
        glm::mat4 lproj = glm::ortho(-25.f, 25.f, -25.f, 25.f, near_plane, far_plane);
        glm::mat4 lview = glm::lookAt(glm::vec3(lx, ly, lz),
                                      glm::vec3(0.f),
                                      glm::vec3(0.f, 1.f, 0.f));
        glm::mat4 lmat = lproj * lview;
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        sm_fb_->Bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        shader3_->Bind();
        shader3_->GetUniform("cam_matrix")->SetValue(lmat);
        draw_scene(0.01666f, shader3_, true);
        Framebuffer::BindScreen();
        
        glViewport(0, 0, w, h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader_->Bind();
        shader_->Bind();
        shader_->GetUniform("lightSpace")->SetValue(lmat);
        shader_->GetUniform("diffuseTexture")->SetValue(0);
        shader_->GetUniform("shadowMap")->SetValue(1);
        draw_scene(delta, shader_, false);
//        if (shader2_->IsCompiled())
//        {
//            shader2_->Bind();
//            shader2_->GetUniform("view")->SetValue(camera.GetViewMatrix());
//            shader2_->GetUniform("projection")->SetValue(camera.GetProjMatrix());
//            mesh2_->Draw(SM_PRIM_LINES);
//        }
 
        skybox_shader_->Bind();
        skybox_shader_->GetUniform("projection")->SetValue(camera.GetProjMatrix());
        skybox_shader_->GetUniform("view")->SetValue(glm::mat4(glm::mat3(camera.GetViewMatrix())));
        skybox_->Draw();
        
        Uint64 delay = SDL_GetTicks64() - t_start;
        if (delay < t_ms)
            SDL_Delay(t_ms - delay);
        delta = (float)(SDL_GetTicks64() - t_start) / 1000.f;
        SDL_GL_SwapWindow(window);
    }

    delete shader_;
    delete shader2_;
    delete shader3_;
    delete shader4_;
    delete skybox_shader_;
    delete skybox_;
    delete texture_;
    delete mesh_;
    delete mesh2_;
    delete mesh3_;
    delete model_;
    delete sm_fb_;
    
    Reference::ReportActiveReferences();
 
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

