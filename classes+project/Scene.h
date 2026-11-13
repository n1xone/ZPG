#pragma once

#include "Model.h"
#include "ShaderProgram.h"
#include "DrawableObject.h"
#include "Camera.h"
#include "Light.h"
#include "Texture2D.h"
#include "Cubemap.h"

#include <array>
#include <optional>
#include <memory>
#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>

class Scene
{
public:
    Scene();
    void update(float movementX, float movementY, float scaleChange, float time, float mouseX,
                float mouseY);
    bool setScene(int scene);
    void draw(float aspectRatio);

    bool screenToWorld(int x, int y, float depth, int width, int height, glm::vec3& position) const;

    bool addTreeAt(const glm::vec3& position);

    enum class MovingPointResult
    {
        Rejected,
        StartSelected,
        Created
    };
    MovingPointResult setMovingPoint(const glm::vec3& point);
    bool removePlacedObject(unsigned int id);
    bool supportsPlacement() const;

private:
    void loadModels();
    void loadShaders();
    void createForest();
    void createSolarSystem();
    void loadTextures();

    void drawSkyDomeScene();
    void drawTriangleScene();
    void drawSphereScene();
    void drawForestScene();
    void drawLoginScene();
    void drawSolarSystemScene();
    void drawClockScene();
    void drawLightingScene();
    void drawTextureScene();
    void drawSkyboxScene();
    void drawCharactersScene();
    void drawClassroomScene();
    void drawTerrainScene();
    void updateClock();

    // Accept both float arrays and vectors; calculate the buffer size in one place.
    template <typename Vertices>
    void addModel(const std::string& name, const Vertices& vertices, int floatsPerVertex,
                  int attributeOffset = 3)
    {
        models_.try_emplace(name, std::data(vertices), std::size(vertices) * sizeof(float),
                            floatsPerVertex, attributeOffset);
    }

    // Shader file names without the shaders/ prefix or extensions.
    void addShader(const std::string& name, const std::string& vertexName,
                   const std::string& fragmentName = "");

    // File names are relative to textures/, without extensions.
    // Without height: texture only. With height: OBJ model and its texture.
    void loadTextured(const std::string& name, const std::string& fileName,
                      const std::string& extension = "png");
    void loadTextured(const std::string& name, const std::string& fileName, float height,
                      const std::string& textureExtension = "png");
    void drawTexturedModel(const std::string& name, const glm::vec3& position,
                           const glm::vec3& rotationDegrees, const glm::vec3& size,
                           const char* textureName = nullptr);

    void drawSkybox();
    void drawModel(const std::string& name);
    unsigned int allocateDrawID();
    unsigned int allocatePlacedID() const;
    void drawPlacedObjects();

    Camera camera_;
    Light firefly_{glm::vec3(0.0f), glm::vec3(1.0f, 0.95f, 0.6f), 2.5f, 1.0f, 0.7f, 5.0f};

    std::unordered_map<std::string, Model> models_;
    std::unordered_map<std::string, ShaderProgram> shaders_;

    std::vector<DrawableObject> trees_;
    std::vector<DrawableObject> bushes_;
    std::vector<DrawableObject> solarSystem_;
    enum class PlacedKind
    {
        Tree,
        MovingSphere
    };
    struct PlacedObject
    {
        unsigned int id;
        int scene;
        PlacedKind kind;
        glm::vec3 start;
        glm::vec3 end;
        float startTime;
    };
    std::vector<PlacedObject> placedObjects_;
    std::optional<glm::vec3> pendingMovingStart_;

    // First unused ID after the fixed objects in each scene have been drawn.
    std::array<unsigned int, 13> firstPlacedID_{};

    std::unique_ptr<Texture2D> woodTexture_;
    std::unique_ptr<Texture2D> grassTexture_;
    std::unique_ptr<Cubemap> skybox_;
    std::unordered_map<std::string, std::unique_ptr<Texture2D>> textures_;

    float angleY_ = 0.0f;
    float time_ = 0.0f;
    float scale_ = 1.0f;
    int currentScene_ = 1;
    unsigned int nextObjectID_ = 1;
    unsigned int nextDrawID_ = 1;
};
