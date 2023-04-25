//
// Created by losts_n1cs3jj on 10/28/2022.
//

#include "Graphics/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

///////////////////////////////////////////////////////////////////////

static Texture::Sampler* s_BoundSampler = nullptr;

Texture::Sampler::~Sampler()
{
    if (s_BoundSampler == this)
        s_BoundSampler = nullptr;
}

SM_TEXTURE_FILTER Texture::Sampler::GetFilter() const
{
    return filter_;
}

SM_TEXTURE_WRAP Texture::Sampler::GetWrap() const
{
    return wrap_;
}

Texture* Texture::Sampler::GetTexture() const
{
    return texture_;
}

void Texture::Sampler::SetFilter(SM_TEXTURE_FILTER filter)
{
    filter_ = filter;
}

void Texture::Sampler::SetWrap(SM_TEXTURE_WRAP wrap)
{
    wrap_ = wrap;
}

void Texture::Sampler::Bind(int active_index)
{
    if (texture_ == nullptr)
        return;
    texture_->Bind(active_index);
    if (GetTexture() != nullptr)
    {
        // When using mipmaps the filter cannot be changed.
        if (GetTexture()->GetMipMapsType() == SM_TEXTURE_MIPS_NONE)
        {
            // Filter Mode
            GLint filter = filter_ == SM_TEXTURE_FILTER_LINEAR ? GL_LINEAR : GL_NEAREST;
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        }
        // Wrap Mode
        GLint wrap = wrap_ == SM_TEXTURE_WRAP_REPEAT ? GL_REPEAT : GL_CLAMP_TO_EDGE;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
    }
    s_BoundSampler = this;
}

///////////////////////////////////////////////////////////////////////

Texture* Texture::Create()
{
    Texture* texture = new Texture();
    glGenTextures(1, &texture->gl_id_);
    texture->default_sampler_ = texture->CreateSampler(SM_TEXTURE_FILTER_NEAREST,
                                                       SM_TEXTURE_WRAP_CLAMP);
    return texture;
}

Texture::~Texture()
{
    if (default_sampler_ != nullptr)
        default_sampler_->Unref();
    glDeleteTextures(1, &gl_id_);
}

Texture::Sampler* Texture::CreateSampler(
        SM_TEXTURE_FILTER filter,
        SM_TEXTURE_WRAP wrap)
{
    Texture::Sampler* sampler = new Texture::Sampler();
    sampler->texture_ = this;
    sampler->filter_ = filter;
    sampler->wrap_ = wrap;
    return sampler;
}

Texture::Sampler* Texture::GetDefaultSampler() const
{
    return default_sampler_;
}

int Texture::GetWidth() const
{
    return width_;
}

int Texture::GetHeight() const
{
    return height_;
}

int Texture::GetComponents() const
{
    return components_;
}

SM_TEXTURE_MIPS Texture::GetMipMapsType() const
{
    return mip_maps_;
}

SM_TEXTURE_FORMAT Texture::GetFormat() const
{
    return format_;
}

SM_TEXTURE_PXTYPE Texture::GetPxType() const
{
    return px_type_;
}

std::string Texture::GetPath() const
{
    return path_;
}

bool Texture::Load(const std::string_view &path, SM_TEXTURE_MIPS mip_maps)
{
    path_.clear();
    
    // Load image
    stbi_uc* px = stbi_load(path.data(), &width_, &height_, &components_, 0);
    if (!px)
    {
        printf("Texture::Load(): Failed to load file `%s'.\n", path.data());
        return false;
    }
    GLint format = 0;
    px_type_ = SM_TEXTURE_PXTYPE_UBYTE;
    mip_maps_ = mip_maps;
    switch(components_)
    {
        case 1: format = GL_RED; format_ = SM_TEXTURE_FORMAT_RED; break;
        case 2: format = GL_RG; format_ = SM_TEXTURE_FORMAT_RG; break;
        case 3: format = GL_RGB; format_ = SM_TEXTURE_FORMAT_RGB; break;
        case 4: format = GL_RGBA; format_ = SM_TEXTURE_FORMAT_RGBA; break;
        default:
        {
            stbi_image_free(px);
            return false;
        }
    }
    GLint gl_filter = GL_NEAREST;
    switch (mip_maps)
    {
        case SM_TEXTURE_MIPS_LINEAR: gl_filter = GL_LINEAR_MIPMAP_LINEAR; break;
        case SM_TEXTURE_MIPS_NEAREST: gl_filter = GL_LINEAR_MIPMAP_NEAREST; break;
        default: break;
    }
    glBindTexture(GL_TEXTURE_2D, gl_id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format,
                 width_, height_, 0, format, GL_UNSIGNED_BYTE,
                 (const GLvoid*)px);
    if (mip_maps != SM_TEXTURE_MIPS_NONE)
        glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(px);
    path_ = path.data();
    return true;
}

bool Texture::Load(SM_TEXTURE_FORMAT format,
          SM_TEXTURE_PXTYPE px_type,
          int width,
          int height,
          const void* pixels,
          SM_TEXTURE_MIPS mip_maps)
{
    path_.clear();
    
    GLint gl_iformat = 0;
    GLenum gl_format = 0;
    GLenum gl_type = px_type == SM_TEXTURE_PXTYPE_FLOAT ? GL_FLOAT : GL_UNSIGNED_BYTE;
    switch(format)
    {
        case SM_TEXTURE_FORMAT_RED:
            gl_format = GL_RED;
            gl_iformat = GL_R8;
            components_ = 1;
            break;
        case SM_TEXTURE_FORMAT_RG:
            gl_format = GL_RG;
            gl_iformat = GL_RG;
            components_ = 2;
            break;
        case SM_TEXTURE_FORMAT_RGB:
            gl_format = GL_RGB;
            gl_iformat = GL_RGB;
            components_ = 3;
            break;
        case SM_TEXTURE_FORMAT_RGBA:
            gl_format = GL_RGBA;
            gl_iformat = GL_RGBA;
            components_ = 4;
            break;
        case SM_TEXTURE_FORMAT_RGBA16F:
            gl_format = GL_RGBA;
            gl_iformat = GL_RGBA16F;
            components_ = 4;
            break;
        case SM_TEXTURE_FORMAT_RGBA16F_RGB:
            gl_format = GL_RGB;
            gl_iformat = GL_RGBA16F;
            components_ = 4;
            break;
        case SM_TEXTURE_FORMAT_DEPTH:
            gl_format = GL_DEPTH_COMPONENT;
            gl_iformat = GL_DEPTH_COMPONENT;
            components_ = 1;
            break;
    }
    format_ = format;
    px_type_ = px_type;
    width_ = width;
    height_ = height;
    mip_maps_ = mip_maps;
    GLint gl_filter = GL_NEAREST;
    switch (mip_maps)
    {
        case SM_TEXTURE_MIPS_LINEAR: gl_filter = GL_LINEAR_MIPMAP_LINEAR; break;
        case SM_TEXTURE_MIPS_NEAREST: gl_filter = GL_LINEAR_MIPMAP_NEAREST; break;
        default: break;
    }
    glBindTexture(GL_TEXTURE_2D, gl_id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, gl_iformat,
                 width_, height_, 0,
                 gl_format, gl_type, pixels);
    return true;
}

void Texture::Bind(int active_index)
{
    glActiveTexture(GL_TEXTURE0 + active_index);
    glBindTexture(GL_TEXTURE_2D, gl_id_);
}
