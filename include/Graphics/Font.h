//
// Created by losts_n1cs3jj on 11/6/2022.
//

#ifndef SM3_FONT_H
#define SM3_FONT_H

#include <string>
#include <vector>
#include <map>
#include <malloc.h>

#include <glm/vec2.hpp>
#include <freetype/freetype.h>

#include "Core/Reference.h"
#include "Texture.h"
#include "Core/Util.h"

#define FONT_ATLAS_SIZE 1024

struct Glyph
{
    unsigned int code{0};
    int index{0};
    int x{0};
    int y{0};
    int width{0};
    int height{0};
    int bearing_x{0};
    int bearing_y{0};
    unsigned int advance_x{0};
};

class Font : public Reference
{
public:
    CLASS_NAME("Font")
    
    static void InitFreetype();
    static void DoneFreetype();
    
    static Font* Create();
    ~Font();
    
    bool Load(const std::string_view& view, const char* utf8_enc_glyph_list);
    bool Load(const std::string_view& view);
    
    glm::vec2 ComputeSize(const char* utf8_text) const;
    glm::vec2 ComputeSize(const char* utf8_text, int count) const;
    
    int GetPixelHeight() const;
     
    Texture* GetAtlas(int index) const;
    int GetAtlasCount() const;
    
    Glyph GetGlyph(int index) const;
    Glyph GetGlyphFromCode(unsigned int code) const;
    int GetGlyphCount() const;
private:
    Font() = default;
    
    int pixel_height_{14};
    std::vector<Glyph> glyphs_;
    std::vector<Texture*> atlas_;
};

#endif //SM3_FONT_H
