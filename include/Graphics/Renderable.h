//
// Created by losts_n1cs3jj on 11/5/2022.
//

#ifndef SM3_RENDERABLE_H
#define SM3_RENDERABLE_H

#include "Graphics/Material.h"
#include "Components/Transform.h"

class Scene;

class Renderable : public Transform
{
public:
    CLASS_NAME("Renderable")

    virtual ~Renderable();
    
    void SetMaterial(Material* material);
    Material* GetMaterial();
    
    virtual void Render(Scene* scene);
protected:
    Renderable() = default;
    
    Material* material_{nullptr};
};

#endif //SM3_RENDERABLE_H
