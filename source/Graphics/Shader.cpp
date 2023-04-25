//
// Created by losts_n1cs3jj on 10/28/2022.
//

#include "Graphics/Shader.h"

#include <iostream>
#include <fstream>

static Uniform invalid_uniform;

Shader *Shader::Create()
{
    return new Shader();
}

Shader::~Shader()
{
    for (auto& uniform : uniforms_)
    {
        delete uniform;
    }
    glDeleteProgram(program_);
}

bool Shader::CreateGLShader(GLenum type, GLuint* out_id, const char* source)
{
    GLchar output[512];
    GLsizei output_size = 0;
    GLint status = GL_FALSE;
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        glGetShaderInfoLog(shader, sizeof(output),
                           &output_size, output);
        if (output_size > 1)
        {
            printf("Shader::Compile(): %s\n", output);
            glDeleteShader(shader);
            return false;
        }
    }
    *out_id = shader;
    return true;
}

bool Shader::Compile(const char* vert_source, const char* frag_source)
{
    program_ = glCreateProgram();
    
    GLchar output[512];
    GLsizei output_size = 0;
    GLint status = GL_FALSE;
    
    GLuint sh_vert = 0, sh_frag = 0;
    if (!CreateGLShader(GL_VERTEX_SHADER, &sh_vert, vert_source))
        return false;
    if (!CreateGLShader(GL_FRAGMENT_SHADER, &sh_frag, frag_source))
        return false;
    glAttachShader(program_, sh_vert);
    glAttachShader(program_, sh_frag);
    glLinkProgram(program_);
    glGetProgramiv(program_, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        glGetProgramInfoLog(program_, sizeof(output),
                            &output_size, output);
        if (output_size > 1)
        {
            printf("Shader::Compile(): %s\n", output);
        }
    }
    glDetachShader(program_, sh_vert);
    glDetachShader(program_, sh_frag);
    glDeleteShader(sh_vert);
    glDeleteShader(sh_frag);
    GLint num_uniforms = 0;
    GLchar u_name[128];
    GLint u_size = 0;
    GLenum u_type = 0;
    glGetProgramiv(program_, GL_ACTIVE_UNIFORMS, &num_uniforms);
    uniforms_.push_back(new Uniform());
    for (GLint i = 0; i < num_uniforms; i++)
    {
        glGetActiveUniform(program_, (GLuint)i, sizeof(u_name),
                           nullptr, &u_size, &u_type, u_name);
        Uniform* uniform = new Uniform();
        uniform->location_ = (int)i;
        uniform->shader_ = this;
        uniform->valid_ = true;
        uniform->name_ = u_name;
        switch (u_type)
        {
            case GL_INT:
                uniform->type_ = SM_UNIFORM_S32;
                break;
            case GL_FLOAT:
                uniform->type_ = SM_UNIFORM_F32;
                break;
            case GL_FLOAT_VEC2:
                uniform->type_ = SM_UNIFORM_F32_2;
                break;
            case GL_FLOAT_VEC3:
                uniform->type_ = SM_UNIFORM_F32_3;
                break;
            case GL_FLOAT_VEC4:
                uniform->type_ = SM_UNIFORM_F32_4;
                break;
            case GL_FLOAT_MAT3:
                uniform->type_ = SM_UNIFORM_F32_3_3;
                break;
            case GL_FLOAT_MAT4:
                uniform->type_ = SM_UNIFORM_F32_4_4;
                break;
            case GL_SAMPLER_2D:
                uniform->type_ = SM_UNIFORM_SAMPLER_2D;
                break;
            default:
                uniform->type_ = SM_UNIFORM_OTHER;
                break;
        }
        uniforms_.push_back(uniform);
    }
    compiled_ = status == GL_TRUE;
    return compiled_;
}

bool Shader::CompileFile(const char* vert_file, const char* frag_file)
{
    FILE* f_vert = fopen(vert_file, "rb");
    if (!f_vert)
        return false;
    FILE* f_frag = fopen(frag_file, "rb");
    if (!f_frag)
    {
        fclose(f_vert);
        return false;
    }

    long size = 0;
    fseek(f_vert, 0, SEEK_END);
    size = ftell(f_vert);
    fseek(f_vert, 0, SEEK_SET);
    char* vert_source = (char*)malloc(size + 1);
    fread(vert_source, 1, size, f_vert);
    vert_source[size] = 0;

    fseek(f_frag, 0, SEEK_END);
    size = ftell(f_frag);
    fseek(f_frag, 0, SEEK_SET);
    char* frag_source = (char*)malloc(size + 1);
    fread(frag_source, 1, size, f_frag);
    frag_source[size] = 0;
    
    fclose(f_vert);
    fclose(f_frag);
    bool result = Compile(vert_source, frag_source);
    free(vert_source);
    free(frag_source);
    return result;
}

bool Shader::IsCompiled()
{
    return compiled_;
}

std::vector<Uniform*> Shader::GetUniforms()
{
    return uniforms_;
}

Uniform* Shader::GetUniform(const char* name)
{
    for (auto& uniform : uniforms_)
    {
        if (uniform->GetName() == name)
            return uniform;
    }
    return &invalid_uniform;
}

void Shader::Bind()
{
    glUseProgram(program_);
}

Shader::Shader(){}
