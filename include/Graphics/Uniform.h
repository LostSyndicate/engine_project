//
// Created by losts_n1cs3jj on 10/30/2022.
//

#ifndef SM3_UNIFORM_H
#define SM3_UNIFORM_H

#include <string>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

enum SM_UNIFORM_TYPE
{
    SM_UNIFORM_OTHER,
    SM_UNIFORM_S32, // int
    SM_UNIFORM_F32, // float
    SM_UNIFORM_F32_2, // vec2(float)
    SM_UNIFORM_F32_3, // vec3(float)
    SM_UNIFORM_F32_4, // vec4(float)
    SM_UNIFORM_F32_3_3, // mat3x3(float)
    SM_UNIFORM_F32_4_4, // mat4x4(float)
    SM_UNIFORM_SAMPLER_2D
};

class Shader;

class Uniform
{
    friend class Shader;
public:
    
    std::string GetName() const;
    SM_UNIFORM_TYPE GetType() const;
    int GetLocation() const;
    Shader* GetShader() const;
    bool IsValid() const;
    
    void SetValue(int value);
    void SetValue(float value);
    void SetValue(const glm::vec2& value);
    void SetValue(const glm::vec3& value);
    void SetValue(const glm::vec4& value);
    void SetValue(const glm::mat3& value, bool transpose = false);
    void SetValue(const glm::mat4& value, bool transpose = false);
private:
    // TODO: Hash the name for faster lookup times. Uniform names never change unless the shader is updated.
    std::string name_;
    SM_UNIFORM_TYPE type_{SM_UNIFORM_S32};
    int location_{0};
    Shader* shader_{nullptr};
    bool valid_{false};
};


#endif //SM3_UNIFORM_H
