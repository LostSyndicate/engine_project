//
// Created by losts_n1cs3jj on 11/1/2022.
//

#include "Graphics/Skybox.h"

#include <stb/stb_image.h>

Skybox* Skybox::Create()
{
    Skybox* skybox = new Skybox();
    skybox->mesh_ = Mesh::Create(SM_MESH_CUBE);
    return skybox;
}

Skybox::~Skybox()
{
    if (mesh_ != nullptr)
        mesh_->Unref();
    glDeleteTextures(1, &gl_id_);
}

bool Skybox::Load(std::vector<std::string> tex_files)
{
    glGenTextures(1, &gl_id_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, gl_id_);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    int width, height, ch;
    for (GLuint i = 0; i < tex_files.size(); i++)
    {
        stbi_uc* pixels = stbi_load(tex_files[i].c_str(), &width, &height, &ch, 0);
        if (!pixels)
        {
            printf("Skybox::Load(): (%u) Failed to load texture `%s'.\n", i, tex_files[i].c_str());
        }
        else
        {
            GLint format = 0;
            switch (ch)
            {
                case 1: format = GL_RED; break;
                case 2: format = GL_RG; break;
                case 3: format = GL_RGB; break;
                case 4: format = GL_RGBA; break;
                default: break;
            }
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
            stbi_image_free(pixels);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return true;
}

void Skybox::Draw()
{
    if (mesh_ == nullptr)
        return;
    
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    glBindTexture(GL_TEXTURE_CUBE_MAP, gl_id_);
    mesh_->Draw();
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glCullFace(GL_FRONT);
}