#include "Camera.h"
#include "ShaderProgram.h"

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <cmath>


Camera::Camera()
    : eye_(0.0f, 0.11f, 3.0f),
      target_(0.0f, 0.0f, -1.0f),
      up_(0.0f, 1.0f, 0.0f),
      viewMatrix_(1.0f),
      projectionMatrix_(1.0f),
      alpha_(glm::radians(90.0f)),
      fi_(glm::radians(-90.0f))
{
    updateTarget();
    updateViewMatrix();
    updateProjectionMatrix();
}


const glm::mat4& Camera::getViewMatrix() const
{
    return viewMatrix_;
}


const glm::mat4& Camera::getProjectionMatrix() const
{
    return projectionMatrix_;
}


void Camera::updateTarget() //camera movement
{
    target_.x = std::sin(alpha_) * std::cos(fi_);
    target_.z = std::sin(alpha_) * std::sin(fi_);
    target_.y = std::cos(alpha_);

    target_ = glm::normalize(target_);
}


void Camera::updateViewMatrix() //camera look to
{
    viewMatrix_ = glm::lookAt(
        eye_,
        eye_ + target_,
        up_
    );
}


void Camera::updateProjectionMatrix()
{
    projectionMatrix_ = glm::perspective(
        glm::radians(60.0f),
        aspectRatio_,
        //1000.0f / 800.0f, // Incorrect: fixed aspect ratio
        0.1f,
        100.0f
    );
}


void Camera::moveForward(float distance) //move forward + back
{
    // Walking on the ground, not flying up/down.
    glm::vec3 forward(target_.x, 0.0f, target_.z);

    if (glm::length(forward) > 0.0001f)
        eye_ += glm::normalize(forward) * distance;

    updateViewMatrix();
    notifyShaders();
}   


void Camera::moveRight(float distance) //move right + left
{
    glm::vec3 forward(target_.x, 0.0f, target_.z);

    if (glm::length(forward) > 0.0001f)
    {
        glm::vec3 right = glm::normalize(
            glm::cross(glm::normalize(forward), up_)
        );

        eye_ += right * distance;
    }

    updateViewMatrix();
    notifyShaders();
}


void Camera::rotate(float mouseX, float mouseY)
{
    const float sensitivity = 0.0025f; 

    fi_ += mouseX * sensitivity;
    alpha_ += mouseY * sensitivity;

    alpha_ = std::clamp(
        alpha_,
        glm::radians(5.0f),
        glm::radians(175.0f)
    );

    const float halfCircle = glm::radians(180.0f);
    const float fullCircle = glm::radians(360.0f);

    if (fi_ > halfCircle)
        fi_ -= fullCircle;
    else if (fi_ < -halfCircle)
        fi_ += fullCircle;

    updateTarget();
    updateViewMatrix();
    notifyShaders();
}


void Camera::setAspectRatio(float aspectRatio)
{
    if (aspectRatio <= 0.0f)
        return;

    if (std::abs(aspectRatio_ - aspectRatio) < 0.0001f)
        return;

    aspectRatio_ = aspectRatio;

    updateProjectionMatrix();
    notifyShaders();
}


void Camera::attachShader(ShaderProgram* shader)
{
    if (!shader)
        return;

    shaders_.push_back(shader);

    shader->setUniform("viewMatrix", viewMatrix_);
    shader->setUniform("projectionMatrix", projectionMatrix_);
}


void Camera::notifyShaders() const
{
    for (ShaderProgram* shader : shaders_)
    {
        shader->setUniform("viewMatrix", viewMatrix_);
        shader->setUniform("projectionMatrix", projectionMatrix_);
    }
}