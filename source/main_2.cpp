#include "Shader.h"
#include "Texture.h"
#include "Model.h"
#include "Framebuffer.h"
#include "Application.h"
#include "Skybox.h"
#include "Camera.h"
#include "UI/UICanvas.h"
#include "UI/UIDropdown.h"
#include "UI/UIButton.h"
#include "UI/UIField.h"
#include "UI/UILabel.h"
#include "UI/UIWindow.h"

#include <random>

#ifdef main
#   undef main
#endif

#define ENABLE_AXIS
#define SHADOW_WIDTH 4096
#define SHADOW_HEIGHT 4096

//////////////////////////////////////////////////////////////////////////////////////
// Static Variables.

static glm::vec3 g_DirLightPos = glm::vec3(6.f, 25.f, -3.f);
static Shader* g_DepthShader = nullptr;
static Shader* g_ShadedShader = nullptr;
static Shader* g_SkyboxShader = nullptr;
static Shader* g_GBufferShader = nullptr;
static Shader* g_ScreenShader = nullptr;
static Shader* g_WorldLinesShader = nullptr;
static Shader* g_UIShader = nullptr;
static Shader* g_DeferredShader = nullptr;

static Input* g_Input = nullptr;
static UIDropdown* g_Dropdown = nullptr;
static UILabel* g_Label = nullptr;
static UIWindow* g_UIWindow = nullptr;
static UICanvas* g_Canvas = nullptr;
static Font* g_Font = nullptr;
static Mesh* g_AxisWireMesh = nullptr;
static Mesh* g_ScreenQuad = nullptr;
static Framebuffer* g_ShadowMap = nullptr;
static Framebuffer* g_DepthMap = nullptr;
static Skybox* g_Skybox = nullptr;
static Model* g_Model = nullptr;
static Camera g_Camera = Camera();
static Application* g_App = nullptr;
static bool g_MouseCaptured = false;

static std::vector<glm::vec3> g_ssaoKernel;
static Texture* g_ssaoNoise = nullptr;

//////////////////////////////////////////////////////////////////////////////////////
// Functions.

void InitScene();
void DestroyScene();
void BeginPoll();
void ProcessEvent(SDL_Event event);
void DrawScene(Shader* shader, bool light_pass);
void DrawFrame(float delta);

//////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
    g_App = smAppCreate("Window", 1280, 720);
    g_App->app_init = InitScene;
    g_App->app_destroy = DestroyScene;
    g_App->app_frame = DrawFrame;
    g_App->app_begin_poll = BeginPoll;
    g_App->app_event = ProcessEvent;
    smAppInit(g_App);
    smAppRun(g_App);
    smAppDestroy(g_App);
    return 0;
}

static float g_lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

void InitScene()
{
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
    std::default_random_engine generator;
    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = (float)i / 64.f;
        scale = g_lerp(0.f, 1.f, scale * scale);
        sample *= scale;
        g_ssaoKernel.push_back(sample);
    }
    std::vector<glm::vec3> ssaoNoise;
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.f);
        ssaoNoise.push_back(noise);
    }
    g_ssaoNoise = Texture::Create();
    g_ssaoNoise->GetDefaultSampler()->SetWrap(SM_TEXTURE_WRAP_REPEAT);
    g_ssaoNoise->Load(SM_TEXTURE_FORMAT_RGBA16F_RGB, SM_TEXTURE_PXTYPE_FLOAT, 4, 4, ssaoNoise.data());
    
    Font::InitFreetype();
    
    g_Input = new Input(g_App->window);
    g_Input->SetSize(g_App->width, g_App->height);
    Input::CreateCursors();
    
    g_Font = Font::Create();
    g_Font->Load("arial.ttf");
    
    g_UIWindow = UIWindow::Create();
    
    g_Dropdown = UIDropdown::Create();
    g_Dropdown->AddOption("Color", "Default");
    g_Dropdown->AddOption("Positions", "Show the positions attachment");
    g_Dropdown->AddOption("Normals", "Show the normals attachment");
    g_Dropdown->AddOption("Albedo", "Show the albedo(textured) attachment");
    g_Dropdown->SetFont(g_Font);
    g_Dropdown->SetSize(glm::vec2(250.f, 28.f));
    g_Dropdown->SetPosition(glm::vec2(0.f, 2.f));
    g_Dropdown->SetExpandMode(UIDropdown::EXPAND_UP);
    g_UIWindow->AddToView(g_Dropdown);
    g_Dropdown->Unref();
    
    float offset_x = g_Dropdown->GetSize().x + 5.f;
    
    UIField* field = UIField::Create();
    field->SetFont(g_Font);
    field->SetPosition(glm::vec2(offset_x, 2.f));
    field->SetSize(glm::vec2(150.f, 28.f));
    field->SetTooltip("UIField");
    g_UIWindow->AddToView(field);
    field->Unref();
    
    offset_x += field->GetSize().x + 5.f;
    
    g_Label = UILabel::Create();
    g_Label->SetFont(g_Font);
    g_Label->SetText("UILabel");
    g_Label->SetPosition(glm::vec2(offset_x, 2.f));
    g_Label->SetTooltip("UILabel");
    g_Label->SetDefaultSize();
    g_UIWindow->AddToView(g_Label);
    g_Label->Unref();
    
    offset_x += g_Label->GetSize().x + 5.f;
    
    UIButton* button = UIButton::Create();
    button->SetFont(g_Font);
    button->SetText("UIButton");
    button->SetTooltip("UIButton");
    button->SetPosition(glm::vec2(offset_x, 2.f));
    button->SetDefaultSize();
    g_UIWindow->AddToView(button);
    button->Unref();
    
    g_ShadedShader = Shader::Create();
    g_ShadedShader->CompileFile("res/shaders/shaded.vert.glsl", "res/shaders/shaded.frag.glsl");
    g_DeferredShader = Shader::Create();
    g_DeferredShader->CompileFile("res/shaders/deferred.vert.glsl", "res/shaders/deferred.frag.glsl");
    g_DepthShader = Shader::Create();
    g_DepthShader->CompileFile("res/shaders/depth.vert.glsl", "res/shaders/depth.frag.glsl");
    g_SkyboxShader = Shader::Create();
    g_SkyboxShader->CompileFile("res/shaders/cubemap.vert.glsl", "res/shaders/cubemap.frag.glsl");
    g_WorldLinesShader = Shader::Create();
    g_WorldLinesShader->CompileFile("res/shaders/world_line.vert.glsl", "res/shaders/world_line.frag.glsl");
    g_GBufferShader = Shader::Create();
    g_GBufferShader->CompileFile("res/shaders/gbuffer.vert.glsl", "res/shaders/gbuffer.frag.glsl");
    g_ScreenShader = Shader::Create();
    g_ScreenShader->CompileFile("res/shaders/screen.vert.glsl", "res/shaders/screen.frag.glsl");
    g_UIShader = Shader::Create();
    g_UIShader->CompileFile("res/shaders/ui.vert.glsl", "res/shaders/ui.frag.glsl");
    g_Skybox = Skybox::Create();
    g_Skybox->Load({
        "res/skybox2/right.png",
        "res/skybox2/left.png",
        "res/skybox2/top.png",
        "res/skybox2/bottom.png",
        "res/skybox2/front.png",
        "res/skybox2/back.png",
    });
    g_Canvas = UICanvas::Create();
    g_Canvas->SetShader(g_UIShader);
    
    g_Model = Model::Create();
    g_Model->Load("res/Sponza/sponza.obj");
    
    g_ShadowMap = Framebuffer::Create(SHADOW_WIDTH, SHADOW_HEIGHT);
    g_ShadowMap->CreateAttachment(SM_ATTACHMENT_TYPE_DEPTH);
    g_ShadowMap->DiscardColorBuffer();
    
    g_DepthMap = Framebuffer::Create(g_App->width, g_App->height);
    g_DepthMap->CreateAttachment(SM_ATTACHMENT_TYPE_DEPTH);
    g_DepthMap->DiscardColorBuffer();
        
    g_AxisWireMesh = Mesh::Create();
    g_AxisWireMesh->SetVertices({
        Vertex(glm::vec3(0, 0, 0), glm::vec3(0), glm::vec2(0)),
        Vertex(glm::vec3(0, 0, 1000), glm::vec3(0), glm::vec2(0)),
        Vertex(glm::vec3(0, 1000, 0), glm::vec3(0), glm::vec2(0)),
        Vertex(glm::vec3(1000, 0, 0), glm::vec3(0), glm::vec2(0))
    });
    g_AxisWireMesh->SetIndices({0,1,0,2,0,3});
    g_ScreenQuad = Mesh::Create();
    g_ScreenQuad->SetVertices({
        Vertex(glm::vec3(-1, -1, 0), glm::vec3(0), glm::vec2(0,0)),
        Vertex(glm::vec3(1, -1, 0), glm::vec3(0), glm::vec2(1,0)),
        Vertex(glm::vec3(1, 1, 0), glm::vec3(0), glm::vec2(1,1)),
        Vertex(glm::vec3(-1, 1, 0), glm::vec3(0), glm::vec2(0,1))
    });
    g_ScreenQuad->SetIndices({0,1,3,1,2,3});
    
    SDL_RaiseWindow(g_App->window);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);
    glViewport(0, 0, 1280, 720);
}

void DestroyScene()
{
    Input::DestroyCursors();
    delete g_Input;
    g_UIWindow->Unref();
    g_Canvas->Unref();
    g_DepthShader->Unref();
    g_DeferredShader->Unref();
    g_ShadedShader->Unref();
    g_SkyboxShader->Unref();
    g_WorldLinesShader->Unref();
    g_UIShader->Unref();
    g_GBufferShader->Unref();
    g_ScreenShader->Unref();
    g_Font->Unref();
    g_AxisWireMesh->Unref();
    g_ScreenQuad->Unref();
    g_ShadowMap->Unref();
    g_DepthMap->Unref();
    g_Skybox->Unref();
    g_Model->Unref();
    g_ssaoNoise->Unref();
    
    Font::DoneFreetype();
    Reference::ReportActiveReferences();
}

void BeginPoll()
{
    g_Input->BeginFrame(g_App->delta, g_App->elapsed);
}

void ProcessEvent(SDL_Event event)
{
    SM_MOUSE btn;
    switch (event.type)
    {
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                g_Input->SetSize(g_App->width, g_App->height);
                g_Camera.SetDimensions((float)g_App->width, (float)g_App->height);
            }
            break;
        case SDL_TEXTINPUT:
            g_Input->SetInputText(event.text.text);
            break;
        case SDL_MOUSEWHEEL:
            g_Input->SetInternalScrollWheel(event.wheel.x, event.wheel.y);
            break;
        case SDL_MOUSEBUTTONDOWN:
            switch (event.button.button)
            {
                case SDL_BUTTON_LEFT: btn = SM_MOUSE_LEFT; break;
                case SDL_BUTTON_MIDDLE: btn = SM_MOUSE_MIDDLE; break;
                case SDL_BUTTON_RIGHT: btn = SM_MOUSE_RIGHT; break;
                default: break;
            }
            g_Input->PressMouse(btn);
            break;
        case SDL_MOUSEBUTTONUP:
            switch (event.button.button)
            {
                case SDL_BUTTON_LEFT: btn = SM_MOUSE_LEFT; break;
                case SDL_BUTTON_MIDDLE: btn = SM_MOUSE_MIDDLE; break;
                case SDL_BUTTON_RIGHT: btn = SM_MOUSE_RIGHT; break;
                default: break;
            }
            g_Input->ReleaseMouse(btn);
            break;
        case SDL_KEYDOWN:
            g_Input->PressKey(event.key.keysym.sym);
            if (event.key.keysym.sym == SDLK_ESCAPE)
                g_MouseCaptured = !g_MouseCaptured;
            SDL_ShowCursor(g_MouseCaptured ? SDL_DISABLE : SDL_ENABLE);
            if (UIElement::GetFocused() == nullptr)
                g_Camera.OnKeyPress((unsigned int)event.key.keysym.sym);
            break;
        case SDL_KEYUP:
            g_Input->ReleaseKey(event.key.keysym.sym);
            if (UIElement::GetFocused() == nullptr)
                g_Camera.OnKeyRelease((unsigned int)event.key.keysym.sym);
            break;
        case SDL_MOUSEMOTION:
            g_Input->SetMouseMoved(true);
            g_Input->SetInternalMousePos(event.motion.x, event.motion.y);
            if (g_MouseCaptured)
                g_Camera.OnMouseMove(event.motion.xrel, event.motion.yrel);
            break;
    }
}

void DrawScene(Shader* shader, bool light_pass)
{
    if (shader->IsCompiled())
    {
        shader->Bind();
        if (!light_pass)
        {
            shader->GetUniform("ambientStrength")->SetValue(0.25f);
            shader->GetUniform("lightPos")->SetValue(g_DirLightPos);
            shader->GetUniform("viewPos")->SetValue(g_Camera.GetPosition());
            shader->GetUniform("view")->SetValue(g_Camera.GetViewMatrix());
            shader->GetUniform("projection")->SetValue(g_Camera.GetProjMatrix());
        }
        shader->GetUniform("model")->SetValue(glm::scale(glm::vec3(0.01f, 0.01f, 0.01f)));
        g_Model->Draw();
    }
}

void DrawFrame(float delta)
{
    if (g_MouseCaptured &&
        (g_App->mouse_x != g_App->width / 2 || g_App->mouse_y != g_App->height / 2))
            SDL_WarpMouseInWindow(g_App->window, g_App->width / 2, g_App->height / 2);
    g_Canvas->SetSize(g_App->width, g_App->height);
    g_Canvas->BeginFrame();
    
    g_Camera.OnUpdate(delta);
    glm::mat4 lightProjection = glm::ortho(
            -25.f, 25.f, -25.f, 25.f,
            1.f, 45.f);
    glm::mat4 lightView = glm::lookAt(g_DirLightPos,
                                      glm::vec3(0.f),
                                      glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 lightSpace = lightProjection * lightView;
    
    glViewport(0, 0, g_App->width, g_App->height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.f);
    
    // Shadow Map
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    g_ShadowMap->Bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    g_DepthShader->Bind();
    g_DepthShader->GetUniform("cam_matrix")->SetValue(lightSpace);
    DrawScene(g_DepthShader, true);
    
    // Depth Map
    /*glViewport(0, 0, g_App->width, g_App->height);
    g_DepthMap->Bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    g_DepthShader->Bind();
    g_DepthShader->GetUniform("cam_matrix")->SetValue(g_Camera.GetProjMatrix() * g_Camera.GetViewMatrix());
    DrawScene(g_DepthShader, true);
    */
    // Shaded Scene
    Framebuffer::BindScreen();
    glViewport(0, 0, g_App->width, g_App->height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    g_ShadedShader->Bind();
    g_ShadedShader->GetUniform("lightSpace")->SetValue(lightSpace);
    g_ShadedShader->GetUniform("diffuseTexture")->SetValue(0);
    g_ShadedShader->GetUniform("shadowMap")->SetValue(1);
    g_ShadowMap->GetAttachment(0)->Bind(1);
    DrawScene(g_ShadedShader, false);
#ifdef ENABLE_AXIS
    g_WorldLinesShader->Bind();
    g_WorldLinesShader->GetUniform("view")->SetValue(g_Camera.GetViewMatrix());
    g_WorldLinesShader->GetUniform("projection")->SetValue(g_Camera.GetProjMatrix());
    g_AxisWireMesh->Draw(SM_PRIM_LINES);
#endif
    // Skybox
    g_SkyboxShader->Bind();
    g_SkyboxShader->GetUniform("projection")->SetValue(g_Camera.GetProjMatrix());
    g_SkyboxShader->GetUniform("view")->SetValue(glm::mat4(glm::mat3(g_Camera.GetViewMatrix())));
    g_Skybox->Draw();
    
    
    // g_ScreenShader->Bind();
    // g_DepthMap->GetAttachment(0)->Bind();
    // g_ScreenQuad->Draw();
    
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    g_Label->SetText("FPS: " + std::to_string(static_cast<int>(1.f / delta)));
    g_UIWindow->Render(g_Canvas, g_Input);
    g_Canvas->EndFrame();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

