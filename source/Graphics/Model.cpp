//
// Created by losts_n1cs3jj on 11/1/2022.
//

#include "Graphics/Model.h"

Model* Model::Create()
{
    return new Model();
}

Model::~Model()
{
    for (auto& mesh : meshes_)
    {
        if (mesh.mesh != nullptr)
            mesh.mesh->Unref();
        if (mesh.sampler != nullptr)
            mesh.sampler->Unref();
    }
    for (auto& pair : textures_)
        pair.second->Unref();
}

bool Model::Load(const char *file)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        printf("Model::Load: (Assimp Error) %s\n", importer.GetErrorString());
        return false;
    }
    std::string str = std::string(file);
    directory_ = str.substr(0, str.find_last_of('/'));
    ProcessNode(scene->mRootNode, scene);
    importer.FreeScene();
    // sort by texture to decrease number of binds.
    std::sort(meshes_.begin(), meshes_.end(), [](ExtendedMesh a, ExtendedMesh b) {
        Texture::Sampler* s1 = a.sampler,
                        * s2 = b.sampler;
        return (s1 == nullptr || s2 == nullptr) ? false : s1->GetTexture() > s2->GetTexture();
    });
    return true;
}

void Model::ProcessNode(aiNode *node, const aiScene *scene)
{
    for (unsigned i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ExtendedMesh extinfo = ProcessMesh(mesh, scene);
        memcpy(&extinfo.matrix, &node->mTransformation, 16 * sizeof(float));
        meshes_.push_back(extinfo);
    }
    for (unsigned i = 0; i < node->mNumChildren; i++)
        ProcessNode(node->mChildren[i], scene);
}

Model::ExtendedMesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    ExtendedMesh extinfo;
    Mesh* r_mesh = Mesh::Create();
    extinfo.mesh = r_mesh;
    extinfo.matrix = glm::mat4(1.f);
    extinfo.sampler = nullptr;
    std::vector<Vertex> vertices;
    std::vector<unsigned short> indices;
    for (unsigned i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vert;
        vert.position_ = glm::vec3(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        );
        vert.normal_ = glm::vec3(
            mesh->mNormals[i].x,
            mesh->mNormals[i].y,
            mesh->mNormals[i].z
        );
        if (mesh->mTextureCoords[0])
        {
            vert.uv_ = glm::vec2(
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            );
        }
        vertices.push_back(vert);
    }
    for (unsigned i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned j = 0; j < face.mNumIndices; j++)
            indices.push_back((unsigned short)face.mIndices[j]);
    }
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            aiString tex_path_str;
            aiTextureMapMode map_mode = aiTextureMapMode_Wrap;
            material->GetTexture(aiTextureType_DIFFUSE, 0, &tex_path_str, NULL, NULL, NULL, NULL, &map_mode);
            std::string c_tex_path = ReplaceOccurrences(std::string(tex_path_str.C_Str()), "\\", "/");
            Texture* diffuseTexture = nullptr;
            auto t_index = textures_.find(c_tex_path);
            if (t_index == textures_.end())
            {
                printf("Model::ProcessMesh: Loaded texture `%s'.\n", (directory_ + "/" + c_tex_path).c_str());
                diffuseTexture = Texture::Create();
                diffuseTexture->Load(directory_ + "/" + c_tex_path, SM_TEXTURE_MIPS_LINEAR);
                textures_.emplace(c_tex_path, diffuseTexture);
            }
            else diffuseTexture = t_index->second;
            Texture::Sampler* sampler = diffuseTexture->CreateSampler();
            sampler->SetFilter(SM_TEXTURE_FILTER_LINEAR);
            if (map_mode == aiTextureMapMode_Wrap)
                sampler->SetWrap(SM_TEXTURE_WRAP_REPEAT);
            extinfo.sampler = sampler;
        }
    }
    r_mesh->SetVertices(vertices.data(), (int)vertices.size());
    r_mesh->SetIndices(indices.data(), (int)indices.size());
    return extinfo;
}

void Model::Draw(SM_PRIM_TYPE type)
{
    for (auto& info : meshes_)
    {
        // somehow set info.matrix here...?
        if (info.sampler != nullptr)
            info.sampler->Bind();
        info.mesh->Draw();
    }
}