#include "Scene.h"
#include "ObjLoader.h"

#include <glm/glm.hpp> //for my matrix
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <random>
#include <cmath>
#include <vector>
#include <stdexcept>

#include "DrawableObject.h"
#include "Transformation.h"

#include "models/sphere.h"
#include "models/OpenGL.h"
#include "models/CHE0086.h"
#include "models/alien.h"
#include "models/illuminati.h"
#include "models/tree.h"
#include "models/bushes.h"
#include "models/earth.h"
#include "models/moon.h"
#include "models/sun.h"
#include "models/clock.h"
#include "models/EB408.h"
#include "models/Whiteboard.h"
#include "models/Door.h"
#include "models/Table.h"
#include "models/OrangeChair.h"
#include "models/teren.h"

namespace
{ // created by Nikita Chervinskyi
float triangle[] = {0.0f, 0.666f, 0.0f, 1.0f,  0.0f,    0.0f, 0.5f, -0.333f, 0.0f,
                    0.0f, 1.0f,   0.0f, -0.5f, -0.333f, 0.0f, 0.0f, 0.0f,    1.0f};

float square[] = {-0.6f, 0.6f, 0.0f, 0.0f, 1.0f,  0.0f, 0.6f,  0.6f,  0.0f,
                  0.0f,  1.0f, 0.0f, 0.6f, -0.6f, 0.0f, 0.0f,  1.0f,  0.0f,

                  -0.6f, 0.6f, 0.0f, 0.0f, 1.0f,  0.0f, -0.6f, -0.6f, 0.0f,
                  0.0f,  1.0f, 0.0f, 0.6f, -0.6f, 0.0f, 0.0f,  1.0f,  0.0f};

const float plainUV[] = {
    // Position          Normal            UV
    1.0f,  0.0f, 1.0f, 0.0f, 1.0f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
    1.0f,  0.0f, 1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f,  1.0f,

    -1.0f, 0.0f, 1.0f, 0.0f, 1.0f,  0.0f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,  0.0f,
    1.0f,  0.0f, 0.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f,  1.0f};

std::vector<float> makeBoxVertices()
{
    const glm::vec3 corners[] = {{-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f},
                                 {-0.5f, 0.5f, -0.5f},  {-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f},
                                 {0.5f, 0.5f, 0.5f},    {-0.5f, 0.5f, 0.5f}};
    const int indices[] = {0, 2, 1, 0, 3, 2, 4, 5, 6, 4, 6, 7, 0, 4, 7, 0, 7, 3,
                           1, 2, 6, 1, 6, 5, 3, 7, 6, 3, 6, 2, 0, 1, 5, 0, 5, 4};
    std::vector<float> vertices;
    vertices.reserve(36 * 8);
    for (int index : indices)
    {
        const auto& point = corners[index];
        vertices.insert(vertices.end(), {point.x, point.y, point.z, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f});
    }
    return vertices;
}

} // namespace

Scene::Scene()
{
    loadModels();
    loadShaders();
    createForest();
    createSolarSystem();
    loadTextures();
}

void Scene::loadModels()
{
    addModel("triangle", triangle, 6);
    addModel("sphere", sphere, 6);
    addModel("square", square, 6);
    addModel("login", che0086, 6);
    addModel("openGL", opengl, 6);
    addModel("tree", tree, 6);
    addModel("bushes", bushes, 6);
    addModel("sun", sun, 9);
    addModel("earth", earth, 9);
    addModel("moon", moon, 9);
    addModel("clock", clockModel, 9, 6); // Attribute starts at float 6 in this model.
    addModel("alien", alien, 9);
    addModel("illuminati", illuminati, 9);
    addModel("plainUV", plainUV, 8);
    addModel("classroom", eb408, 8);
    addModel("whiteboard", whiteboard, 8);
    addModel("door", door, 8);
    addModel("table", table, 8);
    addModel("orangeChair", orangeChair, 8);
    const auto boxVertices = makeBoxVertices();
    addModel("classroomBox", boxVertices, 8);
    addModel("teren", terrainVertices, 8);

    // SKYBOX CUBE
    const glm::vec3 cubeCorners[] = {
        {-1.0f, -1.0f, -1.0f},
        {1.0f, -1.0f, -1.0f},
        {1.0f, 1.0f, -1.0f},
        {-1.0f, 1.0f, -1.0f},
        {-1.0f, -1.0f, 1.0f},
        {1.0f, -1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
        {-1.0f, 1.0f, 1.0f}};

    const int cubeIndices[] = {// Back
                               1, 0, 3, 1, 3, 2,
                               // Front
                               4, 5, 6, 4, 6, 7,
                               // Left
                               0, 4, 7, 0, 7, 3,
                               // Right
                               5, 1, 2, 5, 2, 6,
                               // Top
                               3, 7, 6, 3, 6, 2,
                               // Bottom
                               0, 1, 5, 0, 5, 4};

    std::vector<float> skyboxVertices;
    skyboxVertices.reserve(36 * 6);

    for (int index : cubeIndices)
    {
        const glm::vec3& p = cubeCorners[index];

        // Position + unused normal
        skyboxVertices.insert(skyboxVertices.end(), {p.x, p.y, p.z, 0.0f, 0.0f, 0.0f});
    }
    addModel("skyboxCube", skyboxVertices, 6);

    // SKYDOME - hemisphere with UV
    const int segments = 48;
    const int rings = 16;
    const float PI = 3.14159265f;

    std::vector<float> domeVertices;

    auto addDomeVertex = [&](float theta, float phi, float u, float v)
    {
        float x = std::sin(theta) * std::cos(phi);
        float y = std::cos(theta);
        float z = std::sin(theta) * std::sin(phi);

        // Position, normal, UV
        domeVertices.insert(domeVertices.end(), {x, y, z, x, y, z, u, v});
    };

    for (int r = 0; r < rings; ++r)
    {
        float v0 = static_cast<float>(r) / rings;
        float v1 = static_cast<float>(r + 1) / rings;

        float theta0 = v0 * PI * 0.5f;
        float theta1 = v1 * PI * 0.5f;

        for (int s = 0; s < segments; ++s)
        {
            float u0 = static_cast<float>(s) / segments;
            float u1 = static_cast<float>(s + 1) / segments;

            float phi0 = u0 * 2.0f * PI;
            float phi1 = u1 * 2.0f * PI;

            // First triangle
            addDomeVertex(theta0, phi0, u0, 1.0f - v0);
            addDomeVertex(theta1, phi0, u0, 1.0f - v1);
            addDomeVertex(theta1, phi1, u1, 1.0f - v1);

            // Second triangle
            addDomeVertex(theta0, phi0, u0, 1.0f - v0);
            addDomeVertex(theta1, phi1, u1, 1.0f - v1);
            addDomeVertex(theta0, phi1, u1, 1.0f - v0);
        }
    }
    addModel("skyDome", domeVertices, 8);
}

void Scene::loadShaders()
{
    addShader("basic", "Basic");
    addShader("treeShader", "Tree");
    addShader("bushShader", "Bush");
    addShader("sunShader", "Sun");
    addShader("groundShader", "Ground");
    addShader("clockShader", "Clock");
    addShader("ambientShader", "Ambient");
    addShader("blinnShader", "Blinn");
    addShader("PhongShader", "Phong");
    addShader("LambertShader", "Lambert");
    addShader("textureShader", "Texture");
    addShader("mixShader", "Texture", "Mix");
    addShader("skyboxShader", "Skybox");
    addShader("skyDomeShader", "SkyDome");
    addShader("classroomShader", "Classroom");

    for (auto& entry : shaders_) // enable my observer
    {
        camera_.attachShader(&entry.second);
    }
}

void Scene::createForest()
{
    // FOREST
    std::mt19937 generator(42);

    std::uniform_real_distribution<float> randomX(-4.0f, 4.0f);
    std::uniform_real_distribution<float> randomZ(-9.0f, 1.5f);
    std::uniform_real_distribution<float> randomRotation(0.0f, glm::radians(360.0f));

    std::uniform_real_distribution<float> treeScale(0.05f, 0.19f);
    std::uniform_real_distribution<float> bushScale(0.10f, 0.20f);

    auto createForestObjects =
        [&](const char* modelName, const char* shaderName, auto& randomScale, auto& objects)
    {
        for (int i = 0; i < 74; ++i)
        {
            DrawableObject object(&models_.at(modelName), &shaders_.at(shaderName));

            auto transformation = std::make_shared<CompositeTransformation>();

            transformation->add(std::make_shared<Translation>(
                glm::vec3(randomX(generator), 0.0f, randomZ(generator))));

            transformation->add(
                std::make_shared<Rotation>(randomRotation(generator), glm::vec3(0.0f, 1.0f, 0.0f)));

            transformation->add(std::make_shared<Scale>(glm::vec3(randomScale(generator))));

            object.setTransformation(transformation);
            object.setID(nextObjectID_++);
            objects.push_back(object);
        }
    };

    createForestObjects("tree", "treeShader", treeScale, trees_);
    createForestObjects("bushes", "bushShader", bushScale, bushes_);
}

void Scene::createSolarSystem()
{
    // SOLAR SYSTEM
    const glm::vec3 yAxis(0.0f, 1.0f, 0.0f);

    auto systemTransform = std::make_shared<CompositeTransformation>();
    systemTransform->add(
        std::make_shared<Rotation>(glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

    // Common function for creating solar objects
    auto createSolarBody = [&](const char* modelName, const char* shaderName,
                               std::shared_ptr<CompositeTransformation> parent, float rotationSpeed,
                               float size)
    {
        DrawableObject object(&models_.at(modelName), &shaders_.at(shaderName));

        auto transformation = std::make_shared<CompositeTransformation>();

        transformation->add(parent);
        transformation->add(std::make_shared<Rotation>(0.0f, yAxis, rotationSpeed));
        transformation->add(std::make_shared<Scale>(glm::vec3(size)));

        object.setTransformation(transformation);
        object.setID(nextObjectID_++);
        solarSystem_.push_back(object);
    };

    // SUN
    createSolarBody("sun", "ambientShader", systemTransform, 0.4f, 0.22f);

    // EARTH ORBIT
    auto earthPosition = std::make_shared<CompositeTransformation>();
    earthPosition->add(systemTransform);
    earthPosition->add(std::make_shared<Rotation>(0.0f, yAxis, 0.5f));
    earthPosition->add(std::make_shared<Translation>(glm::vec3(0.60f, 0.0f, 0.0f)));

    // EARTH
    createSolarBody("earth", "PhongShader", earthPosition, 1.5f, 0.10f);

    // MOON ORBIT AROUND EARTH
    auto moonPosition = std::make_shared<CompositeTransformation>();
    moonPosition->add(earthPosition);
    moonPosition->add(std::make_shared<Rotation>(0.0f, yAxis, 2.0f));
    moonPosition->add(std::make_shared<Translation>(glm::vec3(0.19f, 0.0f, 0.0f)));

    // MOON
    createSolarBody("moon", "blinnShader", moonPosition, 1.0f, 0.04f);
}

void Scene::loadTextures()
{
    // Paths are relative to textures/, without extensions; PNG is the default.
    // Textured models also specify their height at scale 1.
    loadTextured("shrek", "shrek/shrek", 1.4f);
    loadTextured("fiona", "shrek/fiona", 1.4f);
    loadTextured("toiled", "shrek/toiled", 2.1f, "jpg");
    loadTextured("classroom", "EB408/T_EB408");
    loadTextured("whiteboard", "Whiteboard/T_Whiteboard_ZPG");
    loadTextured("door", "door/T_door_basecolor");
    loadTextured("table", "EB408Models/T_Table_BaseColor");
    loadTextured("orangeChair", "EB408Models/T_OrangeChair_BaseColor");
    loadTextured("teren", "teren/grass");

    // TEXTURES
    woodTexture_ = std::make_unique<Texture2D>("textures/wood.png");
    grassTexture_ = std::make_unique<Texture2D>("textures/grass.jpg");

    skybox_ = std::make_unique<Cubemap>(std::array<std::string, 6>{
        "textures/skybox/posx.jpg", "textures/skybox/negx.jpg", "textures/skybox/posy.jpg",
        "textures/skybox/negy.jpg", "textures/skybox/posz.jpg", "textures/skybox/negz.jpg"});
}

void Scene::addShader(const std::string& name, const std::string& vertexName,
                      const std::string& fragmentName)
{
    const std::string vertexPath = "shaders/" + vertexName + ".vert";
    const std::string fragmentPath =
        "shaders/" + (fragmentName.empty() ? vertexName : fragmentName) + ".frag";
    shaders_.try_emplace(name, vertexPath.c_str(), fragmentPath.c_str());
}

void Scene::loadTextured(const std::string& name, const std::string& fileName,
                         const std::string& extension)
{
    if (textures_.count(name))
        throw std::runtime_error("Texture name already loaded: " + name);
    const std::string path = "textures/" + fileName + "." + extension;
    textures_.emplace(name, std::make_unique<Texture2D>(path.c_str()));
}

void Scene::loadTextured(const std::string& name, const std::string& fileName, float height,
                         const std::string& textureExtension)
{
    if (models_.count(name))
        throw std::runtime_error("Model name already loaded: " + name);

    const std::string objPath = "textures/" + fileName + ".obj";
    auto vertices = loadTexturedObj(objPath.c_str(), height);
    loadTextured(name, fileName, textureExtension);
    addModel(name, vertices, 8);
}

void Scene::drawTexturedModel(const std::string& name, const glm::vec3& position,
                              const glm::vec3& rotationDegrees, const glm::vec3& size,
                              const char* textureName)
{
    const auto modelEntry = models_.find(name);
    if (modelEntry == models_.end())
        throw std::runtime_error("Model not loaded: " + name);

    // By default use the texture registered with this model's name.
    const std::string textureKey = textureName ? textureName : name;
    const auto textureEntry = textures_.find(textureKey);

    if (textureEntry == textures_.end())
        throw std::runtime_error("Texture not loaded: " + textureKey);

    auto model = glm::translate(glm::mat4(1.0f), position);
    model = glm::rotate(model, glm::radians(rotationDegrees.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(rotationDegrees.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(rotationDegrees.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, size);

    auto& shader = shaders_.at("textureShader");
    shader.setUniform("modelMatrix", model);
    shader.setUniform("textureUnitID", 0);
    textureEntry->second->bind(0);
    shader.use();
    modelEntry->second.draw(allocateDrawID());
    ShaderProgram::unuse();
}

void Scene::drawSkybox()
{
    auto& shader = shaders_.at("skyboxShader");
    shader.setUniform("skyboxTexture", 0);
    skybox_->bind(0);

    // Draw at the far plane without overwriting the scene's depth buffer.
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    shader.use();
    drawModel("skyboxCube");
    ShaderProgram::unuse();
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

bool Scene::setScene(int scene)
{
    if (currentScene_ == scene)
        return false;

    if (scene == 11)
        camera_.moveHeight(0.55f);
    else if (currentScene_ == 11)
        camera_.moveHeight(-0.55f);

    pendingMovingStart_.reset();
    currentScene_ = scene;
    scale_ = 1.0f;
    return true;
}

void Scene::update(float movementX, float movementY, float scaleChange, float time, float mouseX,
                   float mouseY)
{
    camera_.rotate(mouseX, mouseY);

    camera_.moveRight(movementX);
    camera_.moveForward(movementY);

    scale_ += scaleChange;

    if (scale_ < 0.1f)
        scale_ = 0.1f;

    angleY_ = time * 0.5f;
    time_ = time;
}

unsigned int Scene::allocateDrawID()
{
    if (nextDrawID_ > 255)
        throw std::out_of_range("Too many objects for the 8-bit stencil buffer");
    return nextDrawID_++;
}

void Scene::drawModel(const std::string& name)
{
    models_.at(name).draw(allocateDrawID());
}
