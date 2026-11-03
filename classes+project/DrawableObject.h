#pragma once

#include "Model.h"
#include "ShaderProgram.h"
#include "Transformation.h"

#include <memory>


class DrawableObject
{
public:
    DrawableObject(
        const Model* model,
        const ShaderProgram* shader
    );

    void setTransformation(
        std::shared_ptr<Transformation> transformation
    );

    void draw(float time) const;

private:
    const Model* model_;
    const ShaderProgram* shader_;

    std::shared_ptr<Transformation> transformation_;
};