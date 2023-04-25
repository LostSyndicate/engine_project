//
// Created by losts_n1cs3jj on 11/1/2022.
//

#ifndef SM3_MODEL_H
#define SM3_MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <map>
#include <string>
#include <vector>

#include "Graphics/Mesh.h"
#include "Core/Util.h"

class Model : public Reference
{
public:
    CLASS_NAME("Model")
    
    static Model* Create();
    ~Model();

    struct ExtendedMesh
    {
        Mesh* mesh;
        Texture::Sampler* sampler;
        glm::mat4 matrix; // transformation
    };

    bool Load(const char* file);
    void Draw(SM_PRIM_TYPE type = SM_PRIM_TRIS);
private:
    void ProcessNode(aiNode* node, const aiScene* scene);
    ExtendedMesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
    Model() = default;
    
    std::string directory_;
    std::map<std::string, Texture*> textures_;
    std::vector<ExtendedMesh> meshes_;
};

#endif //SM3_MODEL_H
