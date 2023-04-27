//
// Created by losts_n1cs3jj on 10/28/2022.
//

#ifndef SM3_TEXTURE_H
#define SM3_TEXTURE_H

#include <vector>
#include <string>

#include <glad/glad.h>

#include "Core/Reference.h"

enum SM_TEXTURE_FORMAT
{
    SM_TEXTURE_FORMAT_RED,
    SM_TEXTURE_FORMAT_RG,
    SM_TEXTURE_FORMAT_RGB,
    SM_TEXTURE_FORMAT_RGBA,
    SM_TEXTURE_FORMAT_RGBA16F,
    SM_TEXTURE_FORMAT_RGBA16F_RGB,
    SM_TEXTURE_FORMAT_DEPTH
};

enum SM_TEXTURE_PXTYPE
{
    SM_TEXTURE_PXTYPE_UBYTE,
    SM_TEXTURE_PXTYPE_FLOAT
};

enum SM_TEXTURE_MIPS
{
    SM_TEXTURE_MIPS_NONE,
    SM_TEXTURE_MIPS_LINEAR,
    SM_TEXTURE_MIPS_NEAREST
};

enum SM_TEXTURE_FILTER
{
    SM_TEXTURE_FILTER_LINEAR,
    SM_TEXTURE_FILTER_NEAREST
};

enum SM_TEXTURE_WRAP
{
    SM_TEXTURE_WRAP_CLAMP,
    SM_TEXTURE_WRAP_REPEAT
};

class Texture : public Reference
{
    friend class Framebuffer;
public:
    CLASS_NAME("Texture")
    
    class Sampler : public Reference
    {
        friend class Texture;
    public:
        CLASS_NAME("Texture::Sampler")
        
        ~Sampler();
        
        SM_TEXTURE_FILTER GetFilter() const;
        SM_TEXTURE_WRAP GetWrap() const;
        Texture* GetTexture() const;
    
        void SetFilter(SM_TEXTURE_FILTER filter);
        void SetWrap(SM_TEXTURE_WRAP wrap);
        
        void Bind(int active_index = 0);
    private:
        Sampler() = default;
        
        SM_TEXTURE_FILTER filter_{SM_TEXTURE_FILTER_NEAREST};
        SM_TEXTURE_WRAP wrap_{SM_TEXTURE_WRAP_CLAMP};
        Texture* texture_{nullptr};
    };
    
public:
    static Texture* Create();
    ~Texture();
    
    Sampler* CreateSampler(SM_TEXTURE_FILTER filter = SM_TEXTURE_FILTER_LINEAR,
                           SM_TEXTURE_WRAP wrap = SM_TEXTURE_WRAP_CLAMP);
    Sampler* GetDefaultSampler() const;
    
    int GetWidth() const;
    int GetHeight() const;
    int GetComponents() const;
    SM_TEXTURE_MIPS GetMipMapsType() const;
    SM_TEXTURE_FORMAT GetFormat() const;
    SM_TEXTURE_PXTYPE GetPxType() const;
    std::string GetPath() const;

    bool Load(const std::string_view& path, SM_TEXTURE_MIPS mip_maps = SM_TEXTURE_MIPS_NONE);
    bool Load(SM_TEXTURE_FORMAT format,
              SM_TEXTURE_PXTYPE px_type,
              int width,
              int height,
              const void* pixels,
              SM_TEXTURE_MIPS mip_maps = SM_TEXTURE_MIPS_NONE);
    
    // Bind the texture. It is recommended to bind to a created or default sampler instead.
    void Bind(int active_index = 0);
    
private:
    Texture() = default;

    Sampler* default_sampler_{nullptr};
    SM_TEXTURE_MIPS mip_maps_{SM_TEXTURE_MIPS_NONE};
    SM_TEXTURE_FORMAT format_{SM_TEXTURE_FORMAT_RED};
    SM_TEXTURE_PXTYPE px_type_{SM_TEXTURE_PXTYPE_UBYTE};
    int width_{0},
        height_{0},
        components_{0};
    std::string path_;
    GLuint gl_id_{0};
};


#endif //SM3_TEXTURE_H
