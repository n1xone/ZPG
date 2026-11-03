#pragma once

#include <glm/glm.hpp>
#include <vector>

class ShaderProgram;

class Camera
{
public:
    Camera();

    const glm::mat4& getViewMatrix() const;
    const glm::mat4& getProjectionMatrix() const;

    void moveForward(float distance);
    void moveRight(float distance);
    void rotate(float mouseX, float mouseY);

    void setAspectRatio(float aspectRatio);

    void attachShader(ShaderProgram* shader); //for observer

private:
    void updateTarget();
    void updateViewMatrix();
    void updateProjectionMatrix();
    void notifyShaders() const;

    glm::vec3 eye_;
    glm::vec3 target_;
    glm::vec3 up_;

    glm::mat4 viewMatrix_;
    glm::mat4 projectionMatrix_;

    float alpha_;
    float fi_;
    float aspectRatio_ = 4.0f / 3.0f;

    std::vector<ShaderProgram*> shaders_;
};