//
// Created by losts_n1cs3jj on 10/28/2022.
//

#ifndef SM3_MESH_H
#define SM3_MESH_H

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>
#include <vector>

#include "Graphics/Texture.h"
#include "Core/Reference.h"

enum SM_MESH_TYPE
{
    SM_MESH_EMPTY,
    SM_MESH_PLANE,
    SM_MESH_CUBE,
    SM_MESH_SPHERE
};

enum SM_PRIM_TYPE
{
    SM_PRIM_TRIS,
    SM_PRIM_LINES,
    SM_PRIM_POINTS
};

struct Vertex
{
    Vertex() = default;
    Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv) :
            position_(position), normal_(normal), uv_(uv) {}
    glm::vec3 position_{0,0,0};
    glm::vec3 normal_{0,0,0};
    glm::vec2 uv_{0,0};
};

// provides a way to store indices and vertices to the GPU, and to render them.
class Mesh : public Reference
{
public:
    CLASS_NAME("Mesh")
    
    static Mesh* Create(SM_MESH_TYPE type = SM_MESH_EMPTY);
    // copies the data from the mesh, then returns a new one.
    static Mesh* Clone(Mesh* mesh);
    ~Mesh();
    
    void SetVertices(const Vertex* vertices,
                     int count);
    void SetIndices(const unsigned short* indices,
                    int count);
    void SetVertices(const std::vector<Vertex>& vertices);
    void SetIndices(const std::vector<unsigned short>& indices);
    void Draw(SM_PRIM_TYPE type = SM_PRIM_TRIS);
    
    //void SetSampler(Texture::Sampler* sampler);
    //Texture::Sampler* GetSampler() const;

private:
    Mesh() = default;
    GLsizei num_indices_{0};
    GLuint  vao_{0},
            vbo_{0},
            ebo_{0};
};

#endif //SM3_MESH_H
