//
// Created by losts_n1cs3jj on 10/31/2022.
//

#include "Graphics/Camera.h"

#include <SDL2/SDL.h>
#include <cstdio>

Camera::Camera() :
    mvstates_{false,false,false,false,false,false},
    width_(1280.f),
    height_(720.f),
    near_plane_(0.1f),
    far_plane_(100.f),
    fov_(70.f),
    yaw_(0.f),
    pitch_(0.f),
    last_mouse_x_(0),
    last_mouse_y_(0),
    position_(0.f,0.f,0.f),
    front_(0.f, 0.f, 1.f),
    right_(1.f, 0.f, 0.f),
    up_(0.f, 1.f, 0.f),
    projection_matrix_(),
    view_matrix_()
{
}

void Camera::OnMouseMove(int rel_x, int rel_y)
{
    LookAdd((float)rel_x * 0.1f, (float)rel_y * -0.1f);
}

void Camera::OnKeyPress(unsigned int key)
{
    switch(key)
    {
        case SDLK_w: mvstates_[0] = true; break;
        case SDLK_a: mvstates_[3] = true; break;
        case SDLK_s: mvstates_[2] = true; break;
        case SDLK_d: mvstates_[1] = true; break;
        case SDLK_q:
        case SDLK_LCTRL:
            mvstates_[4] = true;
            break;
        case SDLK_e:
        case SDLK_SPACE:
            mvstates_[5] = true;
            break;
        default: break;
    }
}

void Camera::OnKeyRelease(unsigned int key)
{
    switch(key)
    {
        case SDLK_w: mvstates_[0] = false; break;
        case SDLK_a: mvstates_[3] = false; break;
        case SDLK_s: mvstates_[2] = false; break;
        case SDLK_d: mvstates_[1] = false; break;
        case SDLK_q:
        case SDLK_LCTRL:
            mvstates_[4] = false;
            break;
        case SDLK_e:
        case SDLK_SPACE:
            mvstates_[5] = false;
            break;
        default: break;
    }
}

void Camera::OnUpdate(float delta)
{
    glm::vec3 sp = glm::vec3(0.1f);
    if (mvstates_[0])
        position_ += front_ * sp;
    if (mvstates_[1])
        position_ += right_ * sp;
    if (mvstates_[2])
        position_ -= front_ * sp;
    if (mvstates_[3])
        position_ -= right_ * sp;
    if (mvstates_[4])
        position_ -= up_ * sp;
    if (mvstates_[5])
        position_ += up_ * sp;
    BuildMatrices();
}

void Camera::SetDimensions(float width, float height)
{
    width_ = width;
    height_ = height;
}

void Camera::SetFOV(float fov)
{
    fov_ = fov;
}

void Camera::SetNearPlane(float near_plane)
{
    near_plane_ = near_plane;
}

void Camera::SetFarPlane(float far_plane)
{
    far_plane_ = far_plane;
}

void Camera::Look(float yaw, float pitch)
{
    yaw_ = yaw;
    pitch_ = pitch;
    if (pitch_ > 89.f)
        pitch_ = 89.f;
    if (pitch_ < -89.f)
        pitch_ = -89.f;
    glm::vec3 front;
    front.x = cosf(glm::radians(yaw_)) * cosf(glm::radians(pitch_));
    front.y = sinf(glm::radians(pitch_));
    front.z = sinf(glm::radians(yaw_)) * cosf(glm::radians(pitch_));
    front_ = glm::normalize(front);
    right_ = glm::normalize(glm::cross(front_, glm::vec3(0.f, 1.f, 0.f)));
    up_ = glm::normalize((glm::cross(right_, front_)));
}

void Camera::LookAdd(float yaw, float pitch)
{
    Look(yaw_ + yaw, pitch_ + pitch);
}

void Camera::BuildMatrices()
{
    projection_matrix_ = glm::perspective(glm::radians(fov_), width_ / height_,
                                          near_plane_, far_plane_);
    view_matrix_ = glm::lookAt(position_, position_ + front_, up_);
}

float Camera::GetYaw() const
{
    return yaw_;
}

float Camera::GetPitch() const
{
    return pitch_;
}

glm::vec3 Camera::GetPosition() const
{
    return position_;
}

glm::mat4 Camera::GetProjMatrix() const
{
    return projection_matrix_;
}

glm::mat4 Camera::GetViewMatrix() const
{
    return view_matrix_;
}
