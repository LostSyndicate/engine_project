//
// Created by losts_n1cs3jj on 11/6/2022.
//

#ifndef SM3_UICANVAS_H
#define SM3_UICANVAS_H

#include <algorithm>
#include <cstdarg>

#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <stdint.h>

#include "Core/Reference.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Font.h"
#include "Core/Util.h"

#define CANVAS_MAX_VERTS_PER_BATCH 512
#define CANVAS_MAX_INDS_PER_BATCH 768 // (quads are mostly used)

#define SM_COL_WHITE 0xFFFFFFFF
#define SM_COL_BLACK 0xFF000000
#define SM_COL_RED 0xFF0000FF
#define SM_COL_GREEN 0xFF00FF00
#define SM_COL_BLUE 0xFFFF0000

struct Vertex2D
{
    Vertex2D() = default;
    Vertex2D(glm::vec2 position, glm::vec2 uv, glm::vec4 color) :
            position_(position), uv_(uv),
            color_( ( static_cast<uint8_t>( color.a / 255.f ) << 24 ) |
                    ( static_cast<uint8_t>( color.b / 255.f ) << 16 ) |
                    ( static_cast<uint8_t>( color.g / 255.f ) << 8 ) |
                      static_cast<uint8_t>( color.r / 255.f ))
            {}
    Vertex2D(glm::vec2 position, glm::vec2 uv, uint32_t color) :
            position_(position), uv_(uv), color_(color){}
    glm::vec2 position_{0,0};
    glm::vec2 uv_{0,0};
    uint32_t color_{0xFF000000};
};

struct Gradient
{
    enum
    {
        NONE,
        TO_TOP,
        TO_BOTTOM,
        TO_LEFT,
        TO_RIGHT
    } type {NONE};
    uint32_t a{0};
    uint32_t b{0};
};

struct CanvasStyle
{
    int z_order{0};
    float line_width{1.f};
    uint32_t color{0xFFFFFFFF};
    Gradient gradient;
};

class UICanvas : public Reference
{
private:
    struct Batch
    {
        Batch() : is_text(false) {}
        bool is_text : 1;
        Texture::Sampler* sampler{nullptr};
        // use vector instead of malloc-ing because the maximum size
        // of the batch is small enough anyway.
        std::vector<Vertex2D> vertices;
        std::vector<unsigned short> indices;
        int z_order{0};
    };
    
public:
    CLASS_NAME("UICanvas")

    static UICanvas* Create();
    ~UICanvas();
    
    CanvasStyle style = CanvasStyle();

    void SetShader(Shader* shader);
    void SetSize(int width, int height);
        
    void ResetStyle();
    void BeginFrame();
    void EndFrame();
    void BatchData(
            Texture::Sampler* sampler,
            int z_order,
            const Vertex2D* vertices,
            int num_vertices,
            const unsigned short* indices,
            int num_indices,
            bool is_text = false);
    void BatchData(
            Texture::Sampler* sampler,
            int z_order,
            std::vector<Vertex2D> vertices,
            std::vector<unsigned short> indices,
            bool is_text = false);
    void AddText(Font* font, float x, float y, const char* fmt, ...);
    void AddImage(Texture::Sampler* sampler, float x, float y);
    void AddImage(Texture::Sampler* sampler, float x, float y, float w, float h);
    void AddLine(float x1, float y1, float x2, float y2);
    void AddRect(float x, float y, float w, float h);
    void AddRectOutline(float x, float y, float w, float h);
    void AddRoundedRect(float x, float y, float w, float h, float r);
    // c (control point) 1x (line start) 2x (line end)
    // step controls the precision of the bezier. a lower step means more lines.
    void AddBezier(float x1, float y1,
                    float x2, float y2,
                    float cx, float cy, float step = 0.2f);
    void AddEllipse(float x, float y, float rx, float ry, int faces = 12);
    Texture* GetBlankTexture() const;
private:
    static void GetGradientColor(Gradient gradient, uint32_t* tl, uint32_t* tr, uint32_t* bl, uint32_t* br);
    static bool SortBatch(const Batch* a, const Batch* b);
    
    UICanvas() = default;
    
    Shader* shader_{nullptr};
    glm::mat4 view_{1.f};
    int width_{0},
        height_{0};
    GLuint vao_{0};
    GLuint vbo_{0};
    GLuint ebo_{0};
    std::vector<Batch*> batches_;
    Batch* current_batch_{nullptr};
    Texture* blank_texture_{nullptr};
};

#endif //SM3_UICANVAS_H
