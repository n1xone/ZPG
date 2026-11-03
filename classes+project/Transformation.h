#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>

class Transformation
{
public:
    virtual ~Transformation() = default;

    virtual glm::mat4 getMatrix(float time) const = 0;
};


class Translation : public Transformation
{
public:
    Translation(const glm::vec3& position);

    glm::mat4 getMatrix(float time) const override;

private:
    glm::vec3 position_;
};


class Scale : public Transformation
{
public:
    Scale(const glm::vec3& scale);

    glm::mat4 getMatrix(float time) const override;

private:
    glm::vec3 scale_;
};


class Rotation : public Transformation
{
public:
    Rotation(float angle, const glm::vec3& axis);
    Rotation(float angle, const glm::vec3& axis, float speed);

    glm::mat4 getMatrix(float time) const override;

private:
    float angle_;
    glm::vec3 axis_;
    float speed_ = 0.0f;
};


class CompositeTransformation : public Transformation
{
public:
    void add(std::shared_ptr<Transformation> transformation);

    glm::mat4 getMatrix(float time) const override;

private:
    std::vector<std::shared_ptr<Transformation>> transformations_;
};