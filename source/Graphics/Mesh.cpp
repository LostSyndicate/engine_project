//
// Created by losts_n1cs3jj on 10/28/2022.
//

#include "Graphics/Mesh.h"
#include "Core/Profiler.h"

static float cube_verts[] = {
        -0.5f, -0.5f, -0.5f,      0.f, 0.f, -1.f,     0.f, 0.f,
         0.5f, -0.5f, -0.5f,      0.f, 0.f, -1.f,     1.f, 0.f,
         0.5f,  0.5f, -0.5f,      0.f, 0.f, -1.f,     1.f, 1.f,
        -0.5f,  0.5f, -0.5f,      0.f, 0.f, -1.f,     0.f, 1.f,

        0.5f, -0.5f, -0.5f,      1.f, 0.f, 0.f,     0.f, 0.f,
        0.5f, -0.5f, 0.5f,       1.f, 0.f, 0.f,     1.f, 0.f,
        0.5f,  0.5f, 0.5f,       1.f, 0.f, 0.f,     1.f, 1.f,
        0.5f,  0.5f, -0.5f,      1.f, 0.f, 0.f,     0.f, 1.f,

        -0.5f, -0.5f, 0.5f,      0.f, 0.f, 1.f,     0.f, 0.f,
        0.5f, -0.5f, 0.5f,       0.f, 0.f, 1.f,     1.f, 0.f,
        0.5f,  0.5f, 0.5f,       0.f, 0.f, 1.f,     1.f, 1.f,
        -0.5f,  0.5f, 0.5f,      0.f, 0.f, 1.f,     0.f, 1.f,

        -0.5f, -0.5f, -0.5f,     -1.f, 0.f, 0.f,     0.f, 0.f,
        -0.5f, -0.5f, 0.5f,      -1.f, 0.f, 0.f,     1.f, 0.f,
        -0.5f,  0.5f, 0.5f,      -1.f, 0.f, 0.f,     1.f, 1.f,
        -0.5f,  0.5f, -0.5f,     -1.f, 0.f, 0.f,     0.f, 1.f,

        -0.5f, -0.5f, -0.5f,     0.f, -1.f, 0.f,     0.f, 0.f,
         0.5f, -0.5f, -0.5f,     0.f, -1.f, 0.f,     1.f, 0.f,
         0.5f, -0.5f,  0.5f,     0.f, -1.f, 0.f,     1.f, 1.f,
        -0.5f, -0.5f,  0.5f,     0.f, -1.f, 0.f,     0.f, 1.f,

        -0.5f, 0.5f, -0.5f,     0.f, 1.f, 0.f,     0.f, 0.f,
         0.5f, 0.5f, -0.5f,     0.f, 1.f, 0.f,     1.f, 0.f,
         0.5f, 0.5f,  0.5f,     0.f, 1.f, 0.f,     1.f, 1.f,
        -0.5f, 0.5f,  0.5f,     0.f, 1.f, 0.f,     0.f, 1.f,
};

// clock-wise indices.
static unsigned short cube_inds[36] = {
        1, 0, 3, 2, 1, 3, // front
        5, 4, 7, 6, 5, 7, // right
        8, 9, 11, 9, 10, 11, // back
        12, 13, 15, 13, 14, 15, // left
        16, 17, 19, 17, 18, 19, // bottom
        21, 20, 23, 22, 21, 23 // top
};

Mesh* Mesh::Create(SM_MESH_TYPE type)
{
    Mesh* mesh = new Mesh();
    glGenVertexArrays(1, &mesh->vao_);
    glGenBuffers(1, &mesh->vbo_);
    glGenBuffers(1, &mesh->ebo_);
    
    glBindVertexArray(mesh->vao_);
    
    float* verts = nullptr;
    unsigned short* inds = nullptr;
    GLsizeiptr s_verts = 0, num_inds = 0;
    if (type == SM_MESH_CUBE)
    {
        verts = cube_verts;
        s_verts = sizeof(cube_verts);
        inds = cube_inds;
        num_inds = 36;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_);
    glBufferData(GL_ARRAY_BUFFER, s_verts, verts, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_inds * (GLsizeiptr)sizeof(unsigned short), inds, GL_DYNAMIC_DRAW);
    mesh->num_indices_ = (int)num_inds;
    
    GLsizeiptr stride = 0;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)stride);
    glEnableVertexAttribArray(0);
    stride += 3 * sizeof(float);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)stride);
    glEnableVertexAttribArray(1);
    stride += 3 * sizeof(float);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)stride);
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
    return mesh;
}

Mesh* Mesh::Clone(Mesh* mesh)
{
    // TODO: This function
    return nullptr;
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &ebo_);
}

void Mesh::SetVertices(const Vertex* vertices, int count)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(count * sizeof(Vertex)),
                 (const void*)vertices, GL_DYNAMIC_DRAW);
}

void Mesh::SetIndices(const unsigned short *indices, int count)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(count * sizeof(unsigned short)),
                 (const void*)indices, GL_DYNAMIC_DRAW);
    num_indices_ = (GLsizei)count;
}

void Mesh::SetVertices(const std::vector<Vertex>& vertices)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(vertices.size() * sizeof(Vertex)),
                 (const void*)vertices.data(), GL_DYNAMIC_DRAW);
}

void Mesh::SetIndices(const std::vector<unsigned short>& indices)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(indices.size() * sizeof(unsigned short)),
                 (const void*)indices.data(), GL_DYNAMIC_DRAW);
    num_indices_ = (GLsizei)indices.size();
}

void Mesh::Draw(SM_PRIM_TYPE type)
{
    PERF_BEG;
    glBindVertexArray(vao_);
    GLenum gl_type = GL_TRIANGLES;
    if (type == SM_PRIM_LINES) gl_type = GL_LINES;
    else if (type == SM_PRIM_POINTS) gl_type = GL_POINTS;
    glDrawElements(gl_type, num_indices_, GL_UNSIGNED_SHORT, nullptr);
    PERF_END;
}

//glm::mat4 Mesh::GetModelMatrix() const
//{
//    return glm::translate(glm::mat4(1.f), position_)
//            * glm::rotate(glm::mat4(1.f), angle_rotation_, axis_rotation_)
//            * glm::scale(glm::mat4(1.f), scale_);
//}
