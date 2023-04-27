//
// Created by losts_n1cs3jj on 10/31/2022.
//

#ifndef SM3_CAMERA_H
#define SM3_CAMERA_H

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>

class Camera
{
public:
    Camera();
    
    void OnMouseMove(int x, int y);
    void OnKeyPress(unsigned int key);
    void OnKeyRelease(unsigned int key);
    void OnUpdate(float delta);
    
    void SetDimensions(float width, float height);
    void SetFOV(float fov);
    void SetNearPlane(float near_plane);
    void SetFarPlane(float far_plane);

    void Look(float yaw, float pitch);
    void LookAdd(float yaw, float pitch);
    
    void BuildMatrices();
    
    float GetYaw() const;
    float GetPitch() const;
    
    glm::vec3 GetPosition() const;
    glm::mat4 GetProjMatrix() const;
    glm::mat4 GetViewMatrix() const;
private:
    bool mvstates_[6];
    float width_,
          height_,
          near_plane_,
          far_plane_,
          fov_,
          yaw_,
          pitch_;
    int last_mouse_x_,
        last_mouse_y_;
    glm::vec3 position_,
              front_,
              right_,
              up_;
    glm::mat4 projection_matrix_,
              view_matrix_;
};


#endif //SM3_CAMERA_H
