//
// Created by losts_n1cs3jj on 11/5/2022.
//

#include "Components/Transform.h"

static const glm::mat4 sMat4_1 = glm::mat4(1.f);

void Transform::SetPosition(const glm::vec3 &position)
{
    position_ = position;
    matrix_ = glm::lookAt(position_, look_at_target_, look_at_up_) *
              glm::scale(sMat4_1, scale_);
}

void Transform::SetScale(const glm::vec3 &scale)
{
    scale_ = scale;
    matrix_ = glm::lookAt(position_, look_at_target_, look_at_up_) *
              glm::scale(sMat4_1, scale_);
}

glm::vec3 Transform::GetPosition() const
{
    return position_;
}

glm::vec3 Transform::GetScale() const
{
    return scale_;
}

glm::vec3 Transform::GetLookAtTarget() const
{
    return look_at_target_;
}

glm::vec3 Transform::GetLookAtUpDir() const
{
    return look_at_up_;
}

glm::mat4 Transform::GetMatrix() const
{
    return matrix_;
}

void Transform::LookAt(const glm::vec3 &target, const glm::vec3 &up)
{
    look_at_target_ = target;
    look_at_up_ = up;
    matrix_ = glm::lookAt(position_, look_at_target_, look_at_up_) *
            glm::scale(sMat4_1, scale_);
}