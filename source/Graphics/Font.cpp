//
// Created by losts_n1cs3jj on 11/6/2022.
//

#include "Graphics/Font.h"

#define STB_RECT_PACK_IMPLEMENTATION
#include <stb/stb_rect_pack.h>

static FT_Library ft_library{nullptr};

void Font::InitFreetype()
{
    FT_Init_FreeType(&ft_library);
}

void Font::DoneFreetype()
{
    FT_Done_FreeType(ft_library);
}

Font* Font::Create()
{
    return new Font();
}

Font::~Font()
{
    for (auto& texture : atlas_)
        texture->Unref();
}

static void CopyPixelsToRegion(
        uint8_t* dst,
        int dst_x,
        int dst_y,
        int dst_w,
        int dst_h,
        const uint8_t* src,
        int src_w,
        int src_h)
{
    if (dst_x + src_w >= dst_w || dst_y + src_h > dst_h || dst_x < 0 || dst_y < 0)
        return;
    const int offset = dst_x + dst_y * dst_w;
    for (int x = 0; x < src_w; x++) {
        for (int y = 0; y < src_h; y++) {
            dst[x + y * dst_w + offset] = src[x + (src_h - 1 - y) * src_w];
        }
    }
}

bool Font::Load(const std::string_view &view, const char* utf8_enc_glyph_list)
{
    FT_Face face;
    if (FT_New_Face(ft_library, view.data(), 0, &face) != FT_Err_Ok)
        return false;
    FT_Set_Pixel_Sizes(face, 0, (FT_UInt)pixel_height_);
    char c = *utf8_enc_glyph_list;
    struct FontBitmap
    {
        uint8_t* pixels;
        int width;
        int height;
        int bx;
        int by;
        unsigned int ax;
    };
    std::map<unsigned int, FontBitmap> bitmaps;
    while (c != 0)
    {
        unsigned int code = 0;
        int count = smGetUTF8CharPoint(utf8_enc_glyph_list, 0, &code);
        if (FT_Load_Char(face, code, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT) == FT_Err_Ok)
        {
            FontBitmap bitmap = FontBitmap();
            bitmap.bx = face->glyph->bitmap_left;
            bitmap.by = face->glyph->bitmap_top;
            bitmap.ax = face->glyph->advance.x;
            bitmap.width = static_cast<int>(face->glyph->bitmap.width);
            bitmap.height = static_cast<int>(face->glyph->bitmap.rows);
            uint8_t* copy = (uint8_t*)malloc(bitmap.width * bitmap.height);
            memcpy(copy, face->glyph->bitmap.buffer, bitmap.width * bitmap.height);
            bitmap.pixels = copy;
            bitmaps.emplace(code, bitmap);
        }
        utf8_enc_glyph_list += count;
        c = *utf8_enc_glyph_list;
    }
    uint8_t* pixels = (uint8_t*)malloc(FONT_ATLAS_SIZE * FONT_ATLAS_SIZE);
    stbrp_context context = {0};
    std::vector<stbrp_rect> rects;
    std::vector<stbrp_rect> temp_rects;
    for (auto& bitmap : bitmaps)
    {
        stbrp_rect rect = {0};
        rect.x = 0;
        rect.y = 0;
        rect.was_packed = 0;
        rect.id = bitmap.first;
        rect.w = bitmap.second.width;
        rect.h = bitmap.second.height;
        rects.push_back(rect);
    }
    int depth = 0;
    while (true)
    {
        if (depth >= 6 || rects.empty()) break;
        memset(&context, 0, sizeof(stbrp_context));
        stbrp_node* nodes = (stbrp_node*)calloc(rects.size(), sizeof(stbrp_node));
        stbrp_init_target(&context, FONT_ATLAS_SIZE, FONT_ATLAS_SIZE, nodes, (int)rects.size());
        int packed = stbrp_pack_rects(&context, rects.data(), (int)rects.size());
        memset(pixels, 0, FONT_ATLAS_SIZE * FONT_ATLAS_SIZE);
        temp_rects = rects;
        rects.clear();
        for (auto& rect : temp_rects)
        {
            if (!rect.was_packed)
                rects.push_back(rect);
            else
            {
                FontBitmap bmp = bitmaps.at(rect.id);
                Glyph glyph;
                glyph.code = rect.id;
                glyph.index = static_cast<int>(atlas_.size());
                glyph.x = rect.x;
                glyph.y = rect.y;
                glyph.width = bmp.width;
                glyph.height = bmp.height;
                glyph.bearing_x = bmp.bx;
                glyph.bearing_y = bmp.by;
                glyph.advance_x = bmp.ax;
                glyphs_.push_back(glyph);
                CopyPixelsToRegion( pixels,
                                    rect.x, rect.y,
                                    FONT_ATLAS_SIZE, FONT_ATLAS_SIZE,
                                    bmp.pixels,
                                    bmp.width, bmp.height);
            }
        }
        if (!temp_rects.empty())
        {
            Texture* atlas = Texture::Create();
            atlas->GetDefaultSampler()->SetFilter(SM_TEXTURE_FILTER_NEAREST);
            atlas->Load(SM_TEXTURE_FORMAT_RED, SM_TEXTURE_PXTYPE_UBYTE, FONT_ATLAS_SIZE, FONT_ATLAS_SIZE, pixels);
            atlas_.push_back(atlas);
        }
        temp_rects.clear();
        depth++;
        free(nodes);
        if (packed) break;
    }
    free(pixels);
    for (auto& bitmap : bitmaps)
        free(bitmap.second.pixels);
    FT_Done_Face(face);
    return true;
}

bool Font::Load(const std::string_view &view)
{
    static const char* const charsets[3] = {
        "0123456789~!@#$%^&*()-_=+[{]}\\|;:'\",<.>/? ",
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNPOQRSTUVWXYZ",
        "ячсмитьбюфывапролджэйцукенгшщзхъёЯЧСМИТЬБЮФЫВАПРОЛДЖЭЙЦУКЕНГШЩЗХЪЁ№"
    };
    return Load(view, (std::string(charsets[0]) + charsets[1] + charsets[2]).c_str());
}

glm::vec2 Font::ComputeSize(const char* utf8_text) const
{
    glm::vec2 size = glm::vec2(0.f, static_cast<float>(pixel_height_));
    int index = 0;
    // TODO: Im too scared of while(true).. pls use a for loop or something...
    while (true)
    {
        unsigned int code;
        int count = smGetUTF8CharPoint(utf8_text, index, &code);
        index += count;
        if (code == 0) break;
        Glyph g = GetGlyphFromCode(code);
        size.x += static_cast<float>(g.advance_x >> 6);
    }
    return size;
}

glm::vec2 Font::ComputeSize(const char* utf8_text, int count) const
{
    glm::vec2 size = glm::vec2(0.f, static_cast<float>(pixel_height_));
    int index = 0;
    while (count > 0)
    {
        unsigned int code;
        int c = smGetUTF8CharPoint(utf8_text, index, &code);
        index += c;
        if (code == 0) break;
        Glyph g = GetGlyphFromCode(code);
        size.x += static_cast<float>(g.advance_x >> 6);
        count--;
    }
    return size;
}

int Font::GetPixelHeight() const
{
    return pixel_height_;
}

Texture* Font::GetAtlas(int index) const
{
    return atlas_[index];
}

int Font::GetAtlasCount() const
{
    return static_cast<int>(atlas_.size());
}

Glyph Font::GetGlyph(int index) const
{
    return glyphs_[index];
}

Glyph Font::GetGlyphFromCode(unsigned int code) const
{
    for (auto& g : glyphs_)
        if (g.code == code)
            return g;
    return Glyph();
}

int Font::GetGlyphCount() const
{
    return static_cast<int>(glyphs_.size());
}