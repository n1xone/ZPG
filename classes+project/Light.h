#pragma once

#include "ShaderProgram.h"
#include <glm/glm.hpp>

class Light
{
public:
    glm::vec3 position{0.0f};
    glm::vec3 color{1.0f};
    float intensity = 1.0f;
    float constant = 1.0f;
    float linear = 0.0f;
    float quadratic = 0.0f;

    void apply(ShaderProgram& shader) const
    {
        shader.setUniform("lightPosition", position.x, position.y, position.z);
        shader.setUniform("lightColor", color.x, color.y, color.z);
        shader.setUniform("lightIntensity", intensity);
        shader.setUniform("lightConstant", constant);
        shader.setUniform("lightLinear", linear);
        shader.setUniform("lightQuadratic", quadratic);
    }
};
