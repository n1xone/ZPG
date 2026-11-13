#pragma once

#include "Model.h"
#include "ShaderProgram.h"
#include "Transformation.h"

#include <memory>

class DrawableObject
{
public:
    DrawableObject(const Model* model, const ShaderProgram* shader);

    void setTransformation(std::shared_ptr<Transformation> transformation);

    void draw(float time) const;

    void setID(unsigned int newID) { id_ = newID; }

    unsigned int getID() const { return id_; }

private:
    const Model* model_;
    const ShaderProgram* shader_;
    unsigned int id_ = 0;

    std::shared_ptr<Transformation> transformation_;
};