//
// Created by losts_n1cs3jj on 10/28/2022.
//

#ifndef SM3_MATERIAL_H
#define SM3_MATERIAL_H

#include <glm/vec3.hpp>

#include "Graphics/Texture.h"
#include "Graphics/Shader.h"

class Material : public Reference
{
public:
    void SetShader(Shader* shader);
    void SetDiffuseSampler(Texture::Sampler* sampler);
    void SetDiffuseColor(const glm::vec3& rgb);
    void SetSpecularSampler(Texture::Sampler* sampler);
    void SetNormalSampler(Texture::Sampler* sampler);

    Shader* GetShader() const;
    Texture::Sampler* GetDiffuseSampler() const;
    glm::vec3 GetDiffuseColor() const;
    Texture::Sampler* GetSpecularSampler() const;
    Texture::Sampler* GetNormalSampler() const;
private:
    Texture::Sampler
            *diffuse_sampler_{nullptr},
            *specular_sampler_{nullptr},
            *normal_sampler_{nullptr};
    glm::vec3 diffuse_color_{1.f};
};


#endif //SM3_MATERIAL_H
