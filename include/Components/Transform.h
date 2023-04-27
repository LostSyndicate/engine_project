//
// Created by losts_n1cs3jj on 11/5/2022.
//

#ifndef SM3_TRANSFORM_H
#define SM3_TRANSFORM_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Core/Reference.h"

/*

If we dont add a hierarchy to the transform:

Node1: 0,0,0 -> Transform(0,0,0)
    Node2: 0,100,0 -> Transform(0,100,0)

Node1 moves:

Node1: 0,10,0 -> Transform(0,10,0)

Apply respective transforms down hierarchy..

Node2: 0,100,0 -> Transform(0,110,0)

HOWEVER... Then Node2's transform gets set to position 0,0,0 through code. Since the transform is unaware of the hierarchy
           the actual position of Node2 now is 0,0,0, which should be 0,10,0.
*/

class Transform : public Reference
{
    friend class Node;
public:
    CLASS_NAME("Transform")

    void SetPosition(const glm::vec3& position);
    void SetScale(const glm::vec3& scale);
    
    glm::vec3 GetPosition() const;
    glm::vec3 GetScale() const;
    glm::vec3 GetLookAtTarget() const;
    glm::vec3 GetLookAtUpDir() const;
    glm::mat4 GetMatrix() const;
    
    void LookAt(const glm::vec3& target, const glm::vec3& up = glm::vec3(0.f, 1.f, 0.f));
private:
    glm::vec3 look_at_target_{0.f,0.f,0.f};
    glm::vec3 look_at_up_{0.f,1.f,0.f};
    glm::vec3 position_{0.f, 0.f, 0.f};
    glm::vec3 scale_{1.f, 1.f, 1.f};
    glm::mat4 matrix_{1.f};
};

#endif //SM3_TRANSFORM_H
