//
// Created by losts_n1cs3jj on 11/5/2022.
//

#include "Graphics/Renderable.h"

Renderable::~Renderable()
{
    if (material_ != nullptr)
        material_->Unref();
}

void Renderable::SetMaterial(Material* material)
{
    if (material != nullptr)
    {
        if (material_ != nullptr)
            material_->Unref();
        material_ = material;
        material_->Ref();
    }
}

Material* Renderable::GetMaterial()
{
    return material_;
}

void Renderable::Render(Scene* scene){}
