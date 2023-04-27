#ifndef SM3_GRAPHICSRENDERER_H
#define SM3_GRAPHICSRENDERER_H

#include <glm/vec3.hpp>
#include "Graphics/Shader.h"
#include "Graphics/Mesh.h"
#include "Graphics/Light.h"
#include "Graphics/Renderable.h"
#include "Graphics/Skybox.h"
#include "Graphics/SortMode.h"

/*
The purpose of this class is to automatically render all lights, cameras, and meshes added to this class.
This reduces the amount of rendering code you have to do.

This class needs to do these in order to function:
    1) Be able to add, and render any 'renderables' (a class that contains a draw function, coupled by a shader- either in the function or a class field).
    2) Capable of rendering lights
    3) Capable of rendering shadows (ie a directional light will produce a shadow at an area.)

For better functionality, we should also consider adding materials to objects.
*/


/*
All renderers be capable of doing these things:

1) Render Geometry
2) Handle Transparent Geometry
3) Apply Lighting
4) Apply Shadowmap

*/

class GraphicsManager;

class RendererImplementation : public Reference
{
protected:
    RendererImplementation() {}
public:
    enum Type
    {
        NONE,
        FORWARD,
        FORWARD_TILED,
        DEFERRED
    };

    Type GetType();

    virtual void Render();
protected:
    GraphicsManager* gfx_{ nullptr };
    Type type_{ NONE };
};

class ForwardTiledRenderer : public RendererImplementation
{
private:
    ForwardTiledRenderer();
public:
    static ForwardTiledRenderer* Create(GraphicsManager* gfx);
};

class ForwardRenderer;
class DeferredRenderer;

class Material : public Reference
{
public:
    static Material* Create();
    ~Material();

    enum TextureIndex
    {
        ALBEDO,
        NORMAL,
        SPECULAR
    };

    glm::vec3 albedoColor;
    glm::vec3 specularColor;
    float specularStrength;
    float opacity;

    // TODO: Consider if 'castShadows' and 'wireframe' should be in the material.
    bool castShadows;
    bool wireframe;

    void SetShader(Shader* shader);
    void SetSampler(Texture::Sampler* sampler, Material::TextureIndex index);

    Shader* GetShader() const;
    Texture::Sampler* GetSampler(Material::TextureIndex index) const;

private:
    Material();

    Texture::Sampler* samplers_[3]; // (0)albedo, (1)normal, (2)specular ...
    Shader* shader_{ nullptr };
};

struct RenderCommand
{
    Mesh* mesh{ nullptr };
    Material* material{ nullptr };
    glm::mat4 pTransform{ nullptr };
    glm::mat4 transform{ 1.f };

    // if set to true, this command will be removed the next time the command is processed.
    bool removeFromQueue{ false };
};

class RenderList
{
public:
    typedef bool (*SortFunc)(RenderCommand* a, RenderCommand* b);

    // this should not be directly modified, since some of the command members use Reference class.
    std::vector<RenderCommand> commands_;

    void QueueCommand(RenderCommand command);
    void SortCommands(SortFunc func);
    void Clear();
    void Render();
};

/*
TODO: Implement a forward+ renderer.
https://www.3dgep.com/forward-plus/
http://www.aortiz.me/2018/12/21/CG.html#part-2
https://www.cse.chalmers.se/~uffe/clustered_shading_preprint.pdf
*/

class GraphicsManager
{
public:
    // Default uniforms that are accessed by the engine.
    // This is not put in the shader to reduce the memory usage, and realistically these uniforms should be named the same throughout all shaders.
    enum DefaultUniforms
    {
        UNIFORM_DEFAULT_TEXTURE,
        UNIFORM_DEFAULT_MODEL,
        UNIFORM_DEFAULT_VIEW,
        UNIFORM_DEFAULT_PROJECTION,
        UNIFORM_DEFAULT_LIGHTSPACE,//temporary
        UNIFORM_DEFAULT_LIGHTPOS,  // temporary
        UNIFORM_DEFAULT_SHADOWMAP,
        UNIFORM_DEFAULT_AMBIENT_STRENGTH,
        UNIFORM_DEFAULT_VIEWPOS,   // temporary
        UNIFORM_DEFAULT_COUNT, // used only to count, not an actual uniform.
    };

    // settings are applied once the frame renders.
    struct Settings
    {
        float ambientStrength{0.5f};
        glm::vec3 ambientColor{ 0.f,0.f,0.f };
        bool renderShadows{ true };
        bool forceWireframe{ false };
        SortMode transparentSortingMode{SortMode::FrontToBack};
    } settings;

    std::string defaultUniformNames[UNIFORM_DEFAULT_COUNT] = {
        "diffuseTexture",   // UNIFORM_DEFAULT_TEXTURE
        "model",            // UNIFORM_DEFAULT_MODEL
        "view",             // UNIFORM_DEFAULT_VIEW
        "projection",       // UNIFORM_DEFAULT_PROJECTION
        "lightSpace",       // UNIFORM_DEFAULT_LIGHTSPACE
        "lightPos",         // UNIFORM_DEFAULT_LIGHTPOS
        "shadowMap",        // UNIFORM_DEFAULT_SHADOWMAP
        "ambientStrength",  // UNIFORM_DEFAULT_AMBIENT_STRENGTH
        "viewPos"           // UNIFORM_DEFAULT_VIEWPOS
    };

    // Consider the following:
    // We have a normal skybox in the scene.
    // Then, we want to overlay stars.
    // To do this couldn't we have 2 skyboxes render at the same time?
    // maybe skyboxes could have 'layers' to them
    Skybox* GetSkybox() const;

    RenderList* GetRenderList();

    RendererImplementation* GetRenderer();
    void SetRenderer(RendererImplementation* renderer);

    void AddLight(Light* light);
    void RemoveLight(Light* light);

    void Render();

    void RenderShadow(Light* light);
    void RenderShadows();
private:
    Skybox* skybox_{ nullptr };
};

void aaa()
{
    GraphicsManager* gfx;

    RenderCommand command;
    command.mesh = nullptr;
    Material *mat = Material::Create();
    gfx->GetRenderList()->QueueCommand();
}

#endif