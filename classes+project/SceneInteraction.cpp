#include "Scene.h"

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <cmath>

bool Scene::screenToWorld(int x, int y, float depth, int width, int height,
                          glm::vec3& position) const
{
    if (width <= 0 || height <= 0 || x < 0 || x >= width || y < 0 || y >= height ||
        !std::isfinite(depth) || depth < 0.0f || depth >= 1.0f)
        return false;

    glm::vec4 viewport(0, 0, width, height);

    position = glm::unProject(glm::vec3(x + 0.5f, y + 0.5f, depth), camera_.getViewMatrix(),
                              camera_.getProjectionMatrix(), viewport);

    return std::isfinite(position.x) && std::isfinite(position.y) && std::isfinite(position.z);
}

bool Scene::supportsPlacement() const
{
    return currentScene_ == 3 || currentScene_ == 11 || currentScene_ == 12;
}

unsigned int Scene::allocatePlacedID() const
{
    if (!supportsPlacement() || firstPlacedID_[currentScene_] == 0)
        return 0;

    // Reuse deleted IDs, keeping fixed scene objects' IDs unchanged.
    for (unsigned int id = 255; id >= firstPlacedID_[currentScene_]; --id)
    {
        const auto used = std::find_if(
            placedObjects_.begin(), placedObjects_.end(), [&](const PlacedObject& object)
            { return object.scene == currentScene_ && object.id == id; });
        if (used == placedObjects_.end())
            return id;
    }
    return 0;
}

bool Scene::addTreeAt(const glm::vec3& position)
{
    const unsigned int id = allocatePlacedID();
    if (id == 0)
        return false;

    placedObjects_.push_back({id, currentScene_, PlacedKind::Tree, position, position, time_});
    return true;
}

Scene::MovingPointResult Scene::setMovingPoint(const glm::vec3& point)
{
    if (!supportsPlacement())
        return MovingPointResult::Rejected;

    if (!pendingMovingStart_)
    {
        if (allocatePlacedID() == 0)
            return MovingPointResult::Rejected;
        pendingMovingStart_ = point;
        return MovingPointResult::StartSelected;
    }

    if (glm::distance(*pendingMovingStart_, point) < 0.1f)
        return MovingPointResult::Rejected;

    const unsigned int id = allocatePlacedID();
    if (id == 0)
        return MovingPointResult::Rejected;

    placedObjects_.push_back(
        {id, currentScene_, PlacedKind::MovingSphere, *pendingMovingStart_, point, time_});
    pendingMovingStart_.reset();
    return MovingPointResult::Created;
}

bool Scene::removePlacedObject(unsigned int id)
{
    const auto object =
        std::find_if(placedObjects_.begin(), placedObjects_.end(), [&](const PlacedObject& item)
                     { return item.scene == currentScene_ && item.id == id; });
    if (object == placedObjects_.end())
        return false;

    placedObjects_.erase(object);
    return true;
}

void Scene::drawPlacedObjects()
{
    if (!supportsPlacement())
        return;

    Light sunlight{glm::vec3(0.0f, 15.0f, -3.0f), glm::vec3(1.0f), 2.5f, 1.0f, 0.0f, 0.0f};
    auto& treeShader = shaders_.at("treeShader");
    sunlight.apply(treeShader);

    for (const auto& object : placedObjects_)
    {
        if (object.scene != currentScene_)
            continue;

        const bool moving = object.kind == PlacedKind::MovingSphere;
        glm::vec3 position = object.start;
        if (moving)
        {
            const float elapsed = std::max(0.0f, time_ - object.startTime);
            const float phase = std::fmod(elapsed * 0.4f, 2.0f);
            const float t = phase <= 1.0f ? phase : 2.0f - phase;
            // P(t) = A + t * (B - A), t in [0, 1], back and forth.
            position += t * (object.end - object.start);
            position.y += 0.20f;
        }

        auto matrix = glm::translate(glm::mat4(1.0f), position);
        matrix = glm::scale(matrix, glm::vec3(moving ? 0.14f : 0.36f));
        auto& shader = moving ? shaders_.at("basic") : treeShader;
        shader.setUniform("modelMatrix", matrix);
        if (moving)
            shader.setUniform("fragmentColor", 1.0f, 0.25f, 0.15f);
        shader.use();
        models_.at(moving ? "sphere" : "tree").draw(object.id);
        ShaderProgram::unuse();
    }
}
