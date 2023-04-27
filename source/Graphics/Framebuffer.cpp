//
// Created by losts_n1cs3jj on 11/6/2022.
//

#include "Graphics/Framebuffer.h"

////////////////////////////////////////////////////////////////////////////////////
// Framebuffer::Attachment

Framebuffer::Attachment::~Attachment()
{
    glDeleteTextures(1, &gl_id_);
}

Framebuffer *Framebuffer::Attachment::GetFramebuffer() const
{
    return framebuffer_;
}

SM_ATTACHMENT_TYPE Framebuffer::Attachment::GetType() const
{
    return type_;
}

int Framebuffer::Attachment::GetIndex() const
{
    return index_;
}

void Framebuffer::Attachment::Bind(int active_index)
{
    glActiveTexture(GL_TEXTURE0 + active_index);
    glBindTexture(GL_TEXTURE_2D, gl_id_);
}

////////////////////////////////////////////////////////////////////////////////////
// Framebuffer

Framebuffer *Framebuffer::Create(int width, int height)
{
    Framebuffer* framebuffer = new Framebuffer();
    framebuffer->width_ = width;
    framebuffer->height_ = height;
    glGenFramebuffers(1, &framebuffer->gl_id_);
    return framebuffer;
}

void Framebuffer::BindScreen()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer()
{
    for (auto& a : attachments_)
        delete a;
    glDeleteFramebuffers(1, &gl_id_);
    glDeleteRenderbuffers(1, &rbo_);
}

Framebuffer::Attachment *
Framebuffer::CreateAttachment(SM_ATTACHMENT_TYPE type,
                              int attachment_index,
                              SM_TEXTURE_FILTER filter,
                              SM_TEXTURE_WRAP wrap,
                              const glm::vec4 &clamp_color)
{
    if (has_depth_attachment_ && type == SM_ATTACHMENT_TYPE_DEPTH)
        return nullptr;
    Attachment* attachment = new Attachment();
    attachment->framebuffer_ = this;
    attachment->attachment_index_ = attachment_index;
    attachment->index_ = (int)attachments_.size();
    attachment->type_ = type;
    GLint gl_ifmt = GL_RGBA;
    GLenum gl_fmt = GL_RGBA;
    GLenum gl_type = GL_UNSIGNED_BYTE;
    GLenum gl_attach = GL_COLOR_ATTACHMENT0 + attachment_index;
    switch (type)
    {
        case SM_ATTACHMENT_TYPE_DEPTH:
            gl_fmt = GL_DEPTH_COMPONENT;
            gl_ifmt = GL_DEPTH_COMPONENT;
            gl_type = GL_FLOAT;
            gl_attach = GL_DEPTH_ATTACHMENT;
            break;
        case SM_ATTACHMENT_TYPE_RGBA16F:
            gl_ifmt = GL_RGBA16F;
            gl_fmt = GL_RGBA;
            gl_type = GL_FLOAT;
            gl_attach = GL_COLOR_ATTACHMENT0 + attachment_index;
            break;
        default: break;
    }
    
    Bind();

    glGenTextures(1, &attachment->gl_id_);
    glBindTexture(GL_TEXTURE_2D, attachment->gl_id_);
    glTexImage2D(GL_TEXTURE_2D, 0, gl_ifmt, width_, height_, 0, gl_fmt, gl_type, nullptr);
    GLint gl_wrap = GL_CLAMP_TO_EDGE;
    GLint gl_filter = GL_NEAREST;
    if (wrap == SM_TEXTURE_WRAP_REPEAT)
        gl_wrap = GL_REPEAT;
    if (filter == SM_TEXTURE_FILTER_LINEAR)
        gl_filter = GL_LINEAR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gl_wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl_wrap);
    if (gl_wrap == GL_CLAMP_TO_EDGE)
    {
        float color[] = {clamp_color.r,clamp_color.g,clamp_color.b,clamp_color.a};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
    }
    glFramebufferTexture2D(GL_FRAMEBUFFER, gl_attach, GL_TEXTURE_2D,
                           attachment->gl_id_, 0);
        
    if (type == SM_ATTACHMENT_TYPE_DEPTH)
        has_depth_attachment_ = true;
    
    attachments_.push_back(attachment);
    GLenum* gl_attachments = (GLenum*)malloc(attachments_.size() * sizeof(GLenum));
    for (auto i = 0; i < attachments_.size(); i++)
    {
        Attachment* a = attachments_[i];
        switch (a->type_)
        {
            case SM_ATTACHMENT_TYPE_DEPTH:
                gl_attachments[i] = GL_DEPTH_ATTACHMENT;
                break;
            case SM_ATTACHMENT_TYPE_RGBA16F:
            case SM_ATTACHMENT_TYPE_RGBA:
                gl_attachments[i] = GL_COLOR_ATTACHMENT0 + a->attachment_index_;
                break;
            default: 
                gl_attachments[i] = GL_NONE;
                break;
        }
    }
    glDrawBuffers(static_cast<GLsizei>(attachments_.size()), gl_attachments);
    free(gl_attachments);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return attachment;
}

void Framebuffer::DiscardColorBuffer()
{
    Bind();
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}

void Framebuffer::AttachRenderBufferDepth()
{
    if (rbo_ == 0)
    {
        Bind();
        // TODO: Abstract this into a separate class, this probably won't work for another framebuffer that uses this.
        glGenRenderbuffers(1, &rbo_);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width_, height_);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_);
    }
}

int Framebuffer::GetWidth() const
{
    return width_;
}

int Framebuffer::GetHeight() const
{
    return height_;
}

Framebuffer::Attachment *Framebuffer::GetAttachment(int index) const
{
    if (index < 0 || index >= attachments_.size())
        return nullptr;
    return attachments_[index];
}

std::vector<Framebuffer::Attachment*> Framebuffer::GetAttachments() const
{
    return attachments_;
}

int Framebuffer::GetAttachmentCount() const
{
    return (int)attachments_.size();
}

void Framebuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, gl_id_);
}
