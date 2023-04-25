//
// Created by losts_n1cs3jj on 11/6/2022.
//

#include "UI/UICanvas.h"
#include "Core/Profiler.h"

UICanvas* UICanvas::Create()
{
    static unsigned char tex_pixels_1x1[] = { 255, 255, 255, 255 };
    UICanvas* canvas = new UICanvas();
    canvas->blank_texture_ = Texture::Create();
    canvas->blank_texture_->Load(
            SM_TEXTURE_FORMAT_RGBA,
            SM_TEXTURE_PXTYPE_UBYTE,
            1, 1, tex_pixels_1x1);
    
    glGenVertexArrays(1, &canvas->vao_);
    glBindVertexArray(canvas->vao_);
    
    glGenBuffers(1, &canvas->vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, canvas->vbo_);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    
    glGenBuffers(1, &canvas->ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, canvas->ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void*)8);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex2D), (const void*)16);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    return canvas;
}

UICanvas::~UICanvas()
{
    if (blank_texture_ != nullptr)
        blank_texture_->Unref();
    if (shader_ != nullptr)
        shader_->Unref();
    for (auto& batch : batches_)
        delete batch;
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &ebo_);
}

void UICanvas::SetShader(Shader* shader)
{
    if (shader_ != shader)
    {
        if (shader_ != nullptr)
            shader_->Unref();
        if (shader != nullptr)
            shader->Ref();
        shader_ = shader;
    }
}

void UICanvas::SetSize(int width, int height)
{
    width_ = width;
    height_ = height;
    view_ = glm::ortho(0.f, (float)width_, 0.f, (float)height_, -1.f, 1.f);
}

void UICanvas::ResetStyle()
{
    style = CanvasStyle();
}

void UICanvas::BeginFrame()
{
    current_batch_ = nullptr;
    for (auto& batch : batches_)
        delete batch;
    batches_.clear();
}

bool UICanvas::SortBatch(const Batch* a, const Batch* b)
{
    return a->z_order < b->z_order;
}

void UICanvas::EndFrame()
{
    if (shader_ == nullptr)
        return;
    std::sort(batches_.begin(), batches_.end(), SortBatch);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    Texture::Sampler *prev_sampler = nullptr;
    shader_->Bind();
    shader_->GetUniform("projection")->SetValue(view_);
    glBindVertexArray(vao_);
    for (auto& batch : batches_)
    {
        shader_->GetUniform("uIsText")->SetValue(batch->is_text ? 1 : 0);
        if (batch->sampler != prev_sampler && batch->sampler != nullptr)
            batch->sampler->Bind();
        prev_sampler = batch->sampler;
        // Buffer Vertices
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(batch->vertices.size() *
                                                                sizeof(Vertex2D)),
                     batch->vertices.data(), GL_DYNAMIC_DRAW);
        // Buffer Indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(batch->indices.size() *
                                                sizeof(unsigned short)),
                     batch->indices.data(), GL_DYNAMIC_DRAW);
        // Draw
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(batch->indices.size()),
                       GL_UNSIGNED_SHORT, nullptr);
    }
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void UICanvas::BatchData(Texture::Sampler *sampler, int z_order, const Vertex2D* vertices,
                         int num_vertices, const unsigned short* indices, int num_indices,
                         bool is_text)
{
    Batch* batch = current_batch_;
    for (int i = 0; i < batches_.size() + 1; i++)
    {
        if (i != 0) batch = batches_[i-1];
        if (batch != nullptr &&
            batch->sampler == sampler && batch->z_order == z_order &&
            batch->vertices.size() + num_vertices < CANVAS_MAX_VERTS_PER_BATCH &&
            batch->indices.size() + num_indices < CANVAS_MAX_INDS_PER_BATCH &&
            batch->is_text == is_text)
        {
            current_batch_ = batch;
            break;
        }
        if (i == batches_.size())
        {
            batch = new Batch();
            batch->sampler = sampler;
            batch->z_order = z_order;
            batch->is_text = is_text;
            current_batch_ = batch;
            batches_.push_back(batch);
            break;
        }
    }
    if (batch == nullptr)
        return;
    // Vertices
    size_t vertsPrevSize = batch->vertices.size();
    batch->vertices.resize(vertsPrevSize + num_vertices);
    for (int i = 0; i < num_vertices; i++)
        batch->vertices[vertsPrevSize + i] = vertices[i];
    // Indices
    size_t indsPrevSize = batch->indices.size();
    batch->indices.resize(indsPrevSize + num_indices);
    for (int i = 0; i < num_indices; i++)
        batch->indices[indsPrevSize + i] = vertsPrevSize + indices[i];
}

void UICanvas::BatchData(
        Texture::Sampler* sampler,
        int z_order,
        std::vector<Vertex2D> vertices,
        std::vector<unsigned short> indices, bool is_text)
{
    BatchData(sampler, z_order,
              vertices.data(),
              static_cast<int>(vertices.size()),
              indices.data(),
              static_cast<int>(indices.size()),
              is_text);
}

void UICanvas::GetGradientColor(Gradient gradient, uint32_t* tl, uint32_t* tr, uint32_t* bl, uint32_t* br)
{
    if (gradient.type != Gradient::NONE)
    {
        switch (gradient.type)
        {
            case Gradient::TO_BOTTOM:
                if (bl != nullptr) *bl = gradient.b;
                if (br != nullptr) *br = gradient.b;
                if (tl != nullptr) *tl = gradient.a;
                if (tr != nullptr) *tr = gradient.a;
                break;
            case Gradient::TO_TOP:
                if (bl != nullptr) *bl = gradient.a;
                if (br != nullptr) *br = gradient.a;
                if (tl != nullptr) *tl = gradient.b;
                if (tr != nullptr) *tr = gradient.b;
                break;
            case Gradient::TO_LEFT:
                if (bl != nullptr) *bl = gradient.b;
                if (tl != nullptr) *tl = gradient.b;
                if (br != nullptr) *br = gradient.a;
                if (tr != nullptr) *tr = gradient.a;
                break;
            case Gradient::TO_RIGHT:
                if (bl != nullptr) *bl = gradient.a;
                if (tl != nullptr) *tl = gradient.a;
                if (br != nullptr) *br = gradient.b;
                if (tr != nullptr) *tr = gradient.b;
                break;
            default: break;
        }
    }
}

void UICanvas::AddText(Font* font, float x, float y, const char* fmt, ...)
{
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    int index = 0;
    char c = buf[0];
    std::map<Texture*, std::pair<std::vector<Vertex2D>,
                                 std::vector<unsigned short>>> groups;
    while (c != 0)
    {
        unsigned int code = 0;
        int count = smGetUTF8CharPoint(buf, index, &code);
        Glyph g = font->GetGlyphFromCode(code);
        if (g.index != -1)
        {
            Texture* tex = font->GetAtlas(g.index);
            
            auto it = groups.find(tex);
            std::pair<std::vector<Vertex2D>, 
                            std::vector<unsigned short>> *data;
            if (it == groups.end())
            {
                groups.emplace(tex, std::pair<std::vector<Vertex2D>,
                                              std::vector<unsigned short>>());
                data = &groups.rbegin()->second;
            }
            else
                data = &it->second;

            std::vector<Vertex2D> *verts = &data->first;
            std::vector<unsigned short> *inds = &data->second;

            int xp = static_cast<int>(x) + g.bearing_x;
            int yp = static_cast<int>(y) - (g.height - g.bearing_y);
            int o = static_cast<int>(verts->size());
            int tx = tex->GetWidth(), ty = tex->GetHeight();
            
            inds->push_back(o);
            inds->push_back(o + 1);
            inds->push_back(o + 2);
            inds->push_back(o + 0);
            inds->push_back(o + 2);
            inds->push_back(o + 3);
            verts->emplace_back(Vertex2D(glm::vec2(xp,           yp),
                                        glm::vec2((float)g.x / (float)tx, (float)g.y / (float)ty), style.color));
            verts->emplace_back(Vertex2D(glm::vec2(xp + g.width, yp),
                                        glm::vec2((float)(g.x + g.width) / (float)tx, (float)g.y / (float)ty), style.color));
            verts->emplace_back(Vertex2D(glm::vec2(xp + g.width, yp + g.height),
                                        glm::vec2((float)(g.x + g.width) / (float)tx, (float)(g.y + g.height) / (float)ty), style.color));
            verts->emplace_back(Vertex2D(glm::vec2(xp,           yp + g.height),
                                        glm::vec2((float)g.x / (float)tx, (float)(g.y + g.height) / (float)ty), style.color));
        }
        x += static_cast<float>(g.advance_x >> 6);
        index += count;
        c = buf[index];
    }
    for (auto& g : groups)
        BatchData(g.first->GetDefaultSampler(), style.z_order, g.second.first, g.second.second, true);
}

void UICanvas::AddImage(Texture::Sampler* sampler, float x, float y, float w, float h)
{
    uint32_t tl = style.color, 
             tr = style.color,
             bl = style.color,
             br = style.color;
    GetGradientColor(style.gradient, &tl, &tr, &bl, &br);
    BatchData(sampler != nullptr ? sampler : blank_texture_->GetDefaultSampler(), style.z_order,
    {
          Vertex2D(glm::vec2(x, y),           glm::vec2(0, 0), bl),
          Vertex2D(glm::vec2(x + w, y),       glm::vec2(1, 0), br),
          Vertex2D(glm::vec2(x + w, y + h),   glm::vec2(1, 1), tr),
          Vertex2D(glm::vec2(x, y + h),       glm::vec2(0, 1), tl)
    }, {0,1,2,0,2,3});
}

void UICanvas::AddImage(Texture::Sampler* sampler, float x, float y)
{
    AddImage(sampler, x, y, (float)sampler->GetTexture()->GetWidth(), (float)sampler->GetTexture()->GetHeight());
}

void UICanvas::AddLine(float x1, float y1, float x2, float y2)
{
    uint32_t tl = style.color, 
             tr = style.color,
             bl = style.color,
             br = style.color;
    GetGradientColor(style.gradient, &tl, &tr, &bl, &br);
    glm::vec2 edge = glm::vec2(x2 - x1, y2 - y1);
    glm::vec2 perp = glm::vec2(-edge.y, edge.x);
    glm::vec2 dir = glm::normalize(perp) * (style.line_width / 2.f);
    Vertex2D verts[4] = {
        Vertex2D(glm::vec2(x1 - dir.x, y1 - dir.y), glm::vec2(0.f), bl),
        Vertex2D(glm::vec2(x1 + dir.x, y1 + dir.y), glm::vec2(0.f), br),
        Vertex2D(glm::vec2(x2 + dir.x, y2 + dir.y), glm::vec2(0.f), tr),
        Vertex2D(glm::vec2(x2 - dir.x, y2 - dir.y), glm::vec2(0.f), tl)
    };
    static unsigned short inds[6] = {1,0,2,2,0,3};
    BatchData(blank_texture_->GetDefaultSampler(), style.z_order, verts, 4, inds, 6);
}

void UICanvas::AddRect(float x, float y, float w, float h)
{
    uint32_t tl = style.color, 
             tr = style.color,
             bl = style.color,
             br = style.color;
    GetGradientColor(style.gradient, &tl, &tr, &bl, &br);
    BatchData(blank_texture_->GetDefaultSampler(), style.z_order,
    {
        Vertex2D(glm::vec2(x, y),           glm::vec2(0, 0), bl),
        Vertex2D(glm::vec2(x + w, y),       glm::vec2(1, 0), br),
        Vertex2D(glm::vec2(x + w, y + h),   glm::vec2(1, 1), tr),
        Vertex2D(glm::vec2(x, y + h),       glm::vec2(0, 1), tl)
    }, {0,1,2,0,2,3});
}

void UICanvas::AddRectOutline(float x, float y, float w, float h)
{
    float t2 = style.line_width / 2.f;
    
    // bottom
    AddLine(x, y - t2, x + w, y - t2);
    // top
    AddLine(x, y + h + t2, x + w, y + h + t2);
    // left
    AddLine(x - t2, y - style.line_width, x - t2, y + h + style.line_width);
    // right
    AddLine(x + w + t2, y - style.line_width, x + w + t2, y + h + style.line_width);
}

void UICanvas::AddRoundedRect(float x, float y, float w, float h, float r)
{
    // circle starts at: right. direction: counter-clockwise.
    // TODO: UICanvas::AddRoundedRect
    //3.14159265358979323846264338
    std::vector<Vertex2D> verts;
    std::vector<unsigned short> inds;
    verts.push_back(Vertex2D(glm::vec2(x,y), glm::vec2(0), 0xFFFFFFFF));
    const float tl_s = 1.570796325f;
    for (int i = 0; i <= 8; i++)
    {
        float t = tl_s + ((float)i / 8.f) * 1.570796325f;
        float rx = x + cosf(t) * r;
        float ry = y + sinf(t) * r;
        verts.push_back(Vertex2D(glm::vec2(rx, ry), glm::vec2(0), 0xFFFFFFFF));
    }
    for (int i = 1; i < verts.size()-1; i++)
    {
        inds.push_back(0);
        inds.push_back(i);
        inds.push_back(i + 1);
    }
    BatchData(blank_texture_->GetDefaultSampler(), style.z_order, verts, inds);
}

void UICanvas::AddBezier(float x1, float y1,
                        float x2, float y2,
                        float cx, float cy, float step)
{
    float lx = x1, ly = y1;
    float t = 0.f;
    for (t = 0.f; t <= 1.f; t += step)
    {
        float t1 = 1.f - t;
        float bx = t1 * (t1 * x1 + t * cx) + t * (t1 * cx + t * x2);
        float by = t1 * (t1 * y1 + t * cy) + t * (t1 * cy + t * y2);
        AddLine(lx, ly, bx, by);
        lx = bx;
        ly = by;
    }
    // draw to the end
    if (t < 1.f) AddLine(lx, ly, x2, y2);
}

void UICanvas::AddEllipse(float x, float y, float rx, float ry, int faces)
{
    if (faces > 4096)
        return;
    if (faces < 3)
        faces = 3;

    const float pi2 = 3.1415926f * 2.f;

    std::vector<Vertex2D> verts;
    std::vector<unsigned short> inds;

    float lx = x + cosf(0) * rx,
          ly = y + sinf(0) * ry;
    for (int i = 1; i <= faces; i++)
    {
        float t = pi2 * (static_cast<float>(i) / static_cast<float>(faces));
        float px = x + cosf(t) * rx;
        float py = y + sinf(t) * ry;
        verts.push_back(Vertex2D(glm::vec2(lx, ly), glm::vec2(0.f), 0xFFFFFFFF));
        verts.push_back(Vertex2D(glm::vec2(px, py), glm::vec2(0.f), 0xFFFFFFFF));
        lx = px;
        ly = py;
    }
    for (int i = 1; i < verts.size() - 1; i++)
    {
        inds.push_back(0);
        inds.push_back(i);
        inds.push_back(i + 1);
    }
    BatchData(blank_texture_->GetDefaultSampler(), style.z_order, verts, inds);
}

Texture* UICanvas::GetBlankTexture() const
{
    return blank_texture_;
}
