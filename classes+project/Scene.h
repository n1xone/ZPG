#pragma once

#include "Model.h"
#include "ShaderProgram.h"
#include "DrawableObject.h"
#include "Camera.h"
#include "Light.h"

#include <string>
#include <unordered_map>
#include <vector>

class Scene {
public:
    Scene();
    void update(float movementX, float movementY, float scaleChange, float time, float mouseX, float mouseY);
    void setScene(int scene);
    void draw(float aspectRatio);

private:
    Camera camera_;
    Light firefly_{glm::vec3(0.0f), glm::vec3(1.0f, 0.95f, 0.6f), 2.5f, 1.0f, 0.7f, 5.0f};

    std::unordered_map<std::string, Model> models_;
    std::unordered_map<std::string, ShaderProgram> shaders_;

    std::vector<DrawableObject> trees_; //arrays of similar objects (1 < _ )
    std::vector<DrawableObject> bushes_;
    std::vector<DrawableObject> solarSystem_;

    float angleY_ = 0.0f ; //rotation angle around Y axis
    float time_ = 0.0f; //speed of rotation
    float scale_ = 1.0f; //scale (*)
    int currentScene_ = 1;
};
