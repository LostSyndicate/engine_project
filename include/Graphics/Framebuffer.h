//
// Created by losts_n1cs3jj on 11/6/2022.
//

#ifndef SM3_FRAMEBUFFER_H
#define SM3_FRAMEBUFFER_H

#include <vector>

#include <glad/glad.h>
#include <glm/vec4.hpp>

#include "Core/Reference.h"
#include "Graphics/Texture.h"

// RGBA16F
// RGBA
// DEPTH


enum SM_ATTACHMENT_TYPE
{
    SM_ATTACHMENT_TYPE_RGBA,
    SM_ATTACHMENT_TYPE_RGBA16F,
    SM_ATTACHMENT_TYPE_DEPTH
};

class Framebuffer : public Reference
{
public:
    CLASS_NAME("Framebuffer")

    class Attachment
    {
        friend class Framebuffer;
    public:
        ~Attachment();

        Framebuffer* GetFramebuffer() const;
        SM_ATTACHMENT_TYPE GetType() const;
        int GetIndex() const;
        void Bind(int active_index = 0);
    private:
        Attachment() = default;

        Framebuffer* framebuffer_{nullptr};
        SM_ATTACHMENT_TYPE type_{SM_ATTACHMENT_TYPE_DEPTH};
        int attachment_index_{0};
        int index_{0};
        GLuint gl_id_{0};
    };

public:
    static Framebuffer* Create(int width, int height);
    static void BindScreen();
    ~Framebuffer();

    Attachment* CreateAttachment(SM_ATTACHMENT_TYPE type,
                       int attachment_index = 0,
                       SM_TEXTURE_FILTER filter = SM_TEXTURE_FILTER_NEAREST,
                       SM_TEXTURE_WRAP wrap = SM_TEXTURE_WRAP_CLAMP,
                       const glm::vec4& clamp_color = glm::vec4(1.f));
    void DiscardColorBuffer();
    void AttachRenderBufferDepth();
    
    int GetWidth() const;
    int GetHeight() const;
    Attachment* GetAttachment(int index) const;
    std::vector<Attachment*> GetAttachments() const;
    int GetAttachmentCount() const;

    void Bind();
private:
    Framebuffer() = default;
    
    GLuint rbo_{0};
    int width_{0};
    int height_{0};
    GLuint gl_id_{0};
    bool has_depth_attachment_{false};
    std::vector<Attachment*> attachments_;
};

#endif //SM3_FRAMEBUFFER_H
