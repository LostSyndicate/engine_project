//
// Created by losts_n1cs3jj on 10/28/2022.
//

#ifndef SM3_SHADER_H
#define SM3_SHADER_H

#include <string>
#include <vector>

#include <glad/glad.h>

#include "Core/Reference.h"
#include "Uniform.h"


class Shader : public Reference
{
public:
    CLASS_NAME("Shader")

    static Shader* Create();
    ~Shader();
    
    bool Compile(const char* vert_source,
                 const char* frag_source);
    bool CompileFile(const char* vert_file,
                 const char* frag_file);
    bool IsCompiled();
    std::vector<Uniform*> GetUniforms();
    Uniform* GetUniform(const char* name);
    
    void Bind();
private:
    static bool CreateGLShader(GLenum type, GLuint* out_id, const char* source);
    
    Shader();
    
    bool compiled_{false};
    std::vector<Uniform*> uniforms_;
    GLuint program_{0};
};


#endif //SM3_SHADER_H
