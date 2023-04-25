//
// Created by losts_n1cs3jj on 11/1/2022.
//

#ifndef SM3_SKYBOX_H
#define SM3_SKYBOX_H

#include <string>
#include <vector>

#include <glad/glad.h>

#include "Graphics/Mesh.h"
#include "Core/Reference.h"

class Skybox : public Reference
{
public:
    CLASS_NAME("Skybox")
    
    static Skybox* Create();
    ~Skybox();
    
    // order: right, left, top, bottom, front, back.
    bool Load(std::vector<std::string> files);
    
    void Draw();
private:
    Skybox() = default;
    
    Mesh* mesh_{nullptr};
    GLuint gl_id_{0};
};


#endif //SM3_SKYBOX_H
