#include "Transformation.h"

#include <glm/gtc/matrix_transform.hpp>


Translation::Translation(const glm::vec3& position)
    : position_(position)
{
}


glm::mat4 Translation::getMatrix(float time) const
{
    return glm::translate(glm::mat4(1.0f), position_);
}


Scale::Scale(const glm::vec3& scale)
    : scale_(scale)
{
}


glm::mat4 Scale::getMatrix(float time) const
{
    return glm::scale(glm::mat4(1.0f), scale_);
}


Rotation::Rotation(float angle, const glm::vec3& axis)
    : angle_(angle), axis_(axis)
{
}


Rotation::Rotation(float angle, const glm::vec3& axis, float speed)
    : angle_(angle), axis_(axis), speed_(speed)
{
}


glm::mat4 Rotation::getMatrix(float time) const
{
    return glm::rotate(
        glm::mat4(1.0f),
        angle_ + time * speed_,
        axis_
    );
}


void CompositeTransformation::add(
    std::shared_ptr<Transformation> transformation)
{
    transformations_.push_back(transformation);
}


glm::mat4 CompositeTransformation::getMatrix(float time) const
{
    glm::mat4 result = glm::mat4(1.0f);

    for (const auto& transformation : transformations_)
    {
        result = result * transformation->getMatrix(time);
    }

    return result;
}