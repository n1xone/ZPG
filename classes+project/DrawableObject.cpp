#include "DrawableObject.h"


DrawableObject::DrawableObject(
    const Model* model,
    const ShaderProgram* shader)
    : model_(model),
      shader_(shader)
{
}


void DrawableObject::setTransformation(
    std::shared_ptr<Transformation> transformation)
{
    transformation_ = transformation;
}


void DrawableObject::draw(float time) const
{
    glm::mat4 matrix = glm::mat4(1.0f);

    if (transformation_)
        matrix = transformation_->getMatrix(time);

    shader_->setUniform("modelMatrix", matrix);

    shader_->use();
    model_->draw();
    ShaderProgram::unuse();
}