//
// Created by losts_n1cs3jj on 10/30/2022.
//

#include "Graphics/Uniform.h"

#include <glad/glad.h>

std::string Uniform::GetName() const
{
    return name_;
}

SM_UNIFORM_TYPE Uniform::GetType() const
{
    return type_;
}

int Uniform::GetLocation() const
{
    return location_;
}

Shader* Uniform::GetShader() const
{
    return shader_;
}

bool Uniform::IsValid() const
{
    return valid_;
}

void Uniform::SetValue(int value)
{
    glUniform1i(location_, value);
}

void Uniform::SetValue(float value)
{
    glUniform1f(location_, value);
}

void Uniform::SetValue(const glm::vec2 &value)
{
    glUniform2f(location_, value.x, value.y);
}

void Uniform::SetValue(const glm::vec3 &value)
{
    glUniform3f(location_, value.x, value.y, value.z);
}

void Uniform::SetValue(const glm::vec4 &value)
{
    glUniform4f(location_, value.x, value.y, value.z, value.w);
}

void Uniform::SetValue(const glm::mat3 &value, bool transpose)
{
    glUniformMatrix3fv(location_, 1, transpose ? GL_TRUE : GL_FALSE, (const GLfloat*)&value);
}

void Uniform::SetValue(const glm::mat4 &value, bool transpose)
{
    glUniformMatrix4fv(location_, 1, transpose ? GL_TRUE : GL_FALSE, (const GLfloat*)&value);
}
