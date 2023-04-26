#include "_Test.h"
#include "Core/NodeManager.h"
#include "Core/Profiler.h"

#define ENABLE_AXIS
#define SHADOW_WIDTH 4096
#define SHADOW_HEIGHT 4096

static Program* g_Program = nullptr;
static bool g_MouseCaptured = false;

#include "Xml/XmlParser.h"
#include <stdio.h>


/*
auto node = CreateNode();

struct Light 
{
    int offset;
    int position; // the light does not care about the node itself. it only needs a position.
    vec4 col;
    mat view;
    //....
};

// assigns an index to light.
Light l = CreateLight();

node->type = NODE_LIGHT;
// since we know the type is a light, we can obtain a clone of the light at the index and update it.
node->type_index = l->offset;

// to update a light:

Light copy = GetLight(l->offset);
copy.col = "something"
UpdateLight(copy, copy->offset);

// when the nodes position updates, 

*/

void PrintXMLNodes(XmlNode* node, int depth)
{
    printf("%s BEGIN_NAME -->%s<-- END_NAME\n", std::string(depth, '\t').c_str(), node->name.c_str());
    for (auto& c : node->attributes)
        printf("%s BEGIN_ATTRIB -->%s=\"%s\"<-- END_ATTRIB\n", std::string(depth + 1, '\t').c_str(), c->name.c_str(), c->content.c_str());
    for (auto c : node->children)
        PrintXMLNodes(c, depth + 1);
}

int main()
{
    XmlParser *parser = XmlParser::Create();
    XmlNode* proot = parser->ParseFile("res/try_me.xml");
    if (parser->GetError() != nullptr)
        printf("%s\n", parser->GetError());
    else
        PrintXMLNodes(proot, 0);
    parser->Unref();

	g_Program = new Program("Test", 1280, 720);
    g_Program->OnInit();
    g_Program->Run();
    g_Program->OnDestroy();
	delete g_Program;

    return 0;
}

void Program::OnInit()
{
    PERF_BEG;
    Font::InitFreetype();
    Input::CreateCursors();

    input_ = new Input(window_);
    input_->SetSize(width_, height_);

    font_ = Font::Create();
    font_->Load("res/arial.ttf");

    ui_window_ = UIWindow::Create();

    UIButton* btn = UIButton::Create();
    btn->SetFont(font_);
    btn->SetText("Test");
    btn->SetTextAlignment(UIElement::A_LEFT);
    btn->SetSize(glm::vec2(56.f, 56.f));
    btn->SetPosition(glm::vec2(300.f, 2.f));
    ui_window_->AddToView(btn);
    btn->Unref();

    UIDropdown* dropdown = UIDropdown::Create();
    dropdown->AddOption("Left"  , "Anchor to the left.");
    dropdown->AddOption("Right" , "Anchor to the right.");
    dropdown->AddOption("Top"   , "Anchor to the top.");
    dropdown->AddOption("Bottom", "Anchor to the bottom.");
    dropdown->AddOption("Middle", "Anchor to the middle.");
    dropdown->SetFont(font_);
    dropdown->SetSize(glm::vec2(250.f, 28.f));
    dropdown->SetPosition(glm::vec2(0.f, 2.f));
    dropdown->SetExpandMode(UIDropdown::EXPAND_UP);
    dropdown->SetData(btn);

    auto selectFn = [](UIDropdown* d, int i){
        UIButton* b = static_cast<UIButton*>(d->GetData());
        if (i == 0)
            b->SetTextAlignment(UIElement::A_LEFT);
        if (i == 1)
            b->SetTextAlignment(UIElement::A_RIGHT);
        if (i == 2)
            b->SetTextAlignment(UIElement::A_TOP);
        if (i == 3)
            b->SetTextAlignment(UIElement::A_BOTTOM);
        if (i == 4)
            b->SetTextAlignment(UIElement::A_MIDDLE);
    };
    dropdown->OnSelectCallback(selectFn);

    ui_window_->AddToView(dropdown);
    dropdown->Unref();

    s_Depth = Shader::Create();
    s_Depth->CompileFile("res/shaders/depth.vert.glsl", "res/shaders/depth.frag.glsl");
    s_Shaded = Shader::Create();
    s_Shaded->CompileFile("res/shaders/shaded.vert.glsl", "res/shaders/shaded.frag.glsl");
    s_Skybox = Shader::Create();
    s_Skybox->CompileFile("res/shaders/cubemap.vert.glsl", "res/shaders/cubemap.frag.glsl");
    s_GBuffer = Shader::Create();
    s_GBuffer->CompileFile("res/shaders/gbuffer.vert.glsl", "res/shaders/gbuffer.frag.glsl");
    s_Screen = Shader::Create();
    s_Screen->CompileFile("res/shaders/screen.vert.glsl", "res/shaders/screen.frag.glsl");
    s_WorldLines = Shader::Create();
    s_WorldLines->CompileFile("res/shaders/world_line.vert.glsl", "res/shaders/world_line.frag.glsl");
    s_Deferred = Shader::Create();
    s_Deferred->CompileFile("res/shaders/deferred.vert.glsl", "res/shaders/deferred.frag.glsl");
    s_UI = Shader::Create();
    s_UI->CompileFile("res/shaders/ui.vert.glsl", "res/shaders/ui.frag.glsl");

    canvas_ = UICanvas::Create();
    canvas_->SetShader(s_UI);

    /*
    <scene>
        <skybox images="res/skybox2/right.png,res/skybox2/left.png,res/skybox2/top.png,res/skybox2/bottom.png,res/skybox2/front.png,res/skybox2/back.png"/>
    </scene>
    */

    skybox_ = Skybox::Create();
    skybox_->Load({
        "res/skybox2/right.png",
        "res/skybox2/left.png",
        "res/skybox2/top.png",
        "res/skybox2/bottom.png",
        "res/skybox2/front.png",
        "res/skybox2/back.png"
    });

    model_ = Model::Create();
    model_->Load("res/Sponza/sponza.obj");

    shadowMap_ = Framebuffer::Create(SHADOW_WIDTH, SHADOW_HEIGHT);
    shadowMap_->CreateAttachment(SM_ATTACHMENT_TYPE_DEPTH);
    shadowMap_->DiscardColorBuffer();

    SDL_RaiseWindow(window_);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);
    glViewport(0, 0, width_, height_);
    PERF_END;
}

void Program::OnDestroy()
{
    delete input_;
    s_Depth->Unref();
    s_Shaded->Unref();
    s_Skybox->Unref();
    s_GBuffer->Unref();
    s_Screen->Unref();
    s_WorldLines->Unref();
    s_Deferred->Unref();
    s_UI->Unref();
    font_->Unref();
    ui_window_->Unref();
    canvas_->Unref();
    shadowMap_->Unref();
    model_->Unref();
    skybox_->Unref();
    // TODO(0): These should be automatically called after everything is cleaned up.
    Input::DestroyCursors();
    Font::DoneFreetype();
    Reference::ReportActiveReferences();
}

void Program::DrawScene(Shader* shader, bool shadow_pass)
{
    PERF_BEG;
    if (shader->IsCompiled())
    {
        shader->Bind();
        if (!shadow_pass)
        {
            shader->GetUniform("ambientStrength")->SetValue(0.25f);
            shader->GetUniform("lightPos")->SetValue(light_pos_);
            shader->GetUniform("viewPos")->SetValue(camera_.GetPosition());
            shader->GetUniform("view")->SetValue(camera_.GetViewMatrix());
            shader->GetUniform("projection")->SetValue(camera_.GetProjMatrix());
        }
        shader->GetUniform("model")->SetValue(glm::scale(glm::vec3(0.01f, 0.01f, 0.01f)));
        model_->Draw();
        shader->GetUniform("model")->SetValue(glm::translate(glm::vec3(10.f, 0.f, 0.f)) * glm::scale(glm::vec3(0.01f, 0.01f, 0.01f)));
        model_->Draw();
    }
    PERF_END;
}

void Program::OnFrame(float delta)
{
    PERF_BEG;
    if (g_MouseCaptured &&
        (mouse_x_ != width_ / 2 || mouse_y_ != height_ / 2))
        SDL_WarpMouseInWindow(window_, width_ / 2, height_ / 2);
    camera_.OnUpdate(delta);

    glm::mat4 lightProjection = glm::ortho(
        -25.f, 25.f, -25.f, 25.f,
        1.f, 45.f);
    glm::mat4 lightView = glm::lookAt(light_pos_,
        glm::vec3(0.f),
        glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 lightSpace = lightProjection * lightView;
    glViewport(0, 0, width_, height_);
    glClearColor(0.0f, 0.0f, 0.0f, 1.f);

    // Shadow
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    shadowMap_->Bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    s_Depth->Bind();
    s_Depth->GetUniform("cam_matrix")->SetValue(lightSpace);
    DrawScene(s_Depth, true);

    // Shaded
    Framebuffer::BindScreen();
    glViewport(0, 0, width_, height_);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    s_Shaded->Bind();
    s_Shaded->GetUniform("lightSpace")->SetValue(lightSpace);
    s_Shaded->GetUniform("diffuseTexture")->SetValue(0);
    s_Shaded->GetUniform("shadowMap")->SetValue(1);
    shadowMap_->GetAttachment(0)->Bind(1);
    DrawScene(s_Shaded, false);

    // Skybox
    s_Skybox->Bind();
    s_Skybox->GetUniform("projection")->SetValue(camera_.GetProjMatrix());
    s_Skybox->GetUniform("view")->SetValue(glm::mat4(glm::mat3(camera_.GetViewMatrix())));
    skybox_->Draw();

    canvas_->SetSize(width_, height_);
    canvas_->BeginFrame();
    ui_window_->Render(canvas_, input_);
    canvas_->AddBezier(100, 100, 200, 200, 150, 250);
    canvas_->EndFrame();
    
    PERF_END;

    canvas_->BeginFrame();
    static float counter = 0.f;
    static std::vector<Profiler::Profile> profiles;
    if (counter >= 0.3f)
    {
        profiles = Profiler::GetProfilesSortedByHighestTime();
        counter = 0.f;
    }
    counter += delta;
    float y = 0.f;
    for (auto& p : profiles)
    {
        if (p.calling_id != 0)
        {
            std::string calling_name;
            for (auto& pp : profiles) {
                if (pp.id == p.calling_id)
                    calling_name = pp.name;
            }
            canvas_->AddText(font_, 0.f, height_ - 50.f - y, "%s: %ums %dx | called by `%s'",
                p.name.c_str(), p.ms, p.calls, calling_name.c_str());
        }
        else
            canvas_->AddText(font_, 0.f, height_ - 50.f - y, "%s: %ums %dx", p.name.c_str(), p.ms, p.calls);
        y += font_->GetPixelHeight();
    }
    canvas_->EndFrame();
}

void Program::OnBeginPoll()
{
	input_->BeginFrame(GetDeltaTime(), GetElapsedTime());
}

void Program::OnEvent(SDL_Event event)
{
    input_->HandleEvent(event);
    switch (event.type)
    {
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                camera_.SetDimensions((float)width_, (float)height_);
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                g_MouseCaptured = !g_MouseCaptured;
            SDL_ShowCursor(g_MouseCaptured ? SDL_DISABLE : SDL_ENABLE);
            if (UIElement::GetFocused() == nullptr)
                camera_.OnKeyPress((unsigned int)event.key.keysym.sym);
            break;
        case SDL_KEYUP:
            if (UIElement::GetFocused() == nullptr)
                camera_.OnKeyRelease((unsigned int)event.key.keysym.sym);
            break;
        case SDL_MOUSEMOTION:
            if (g_MouseCaptured)
                camera_.OnMouseMove(event.motion.xrel, event.motion.yrel);
            break;
    }
}