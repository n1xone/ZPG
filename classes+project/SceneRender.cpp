#include "Scene.h"

#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include <ctime>

void Scene::updateClock()
{
    // Current local system time
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm localTime = *std::localtime(&currentTime);

    // Milliseconds for smooth movement
    float milliseconds =
        static_cast<float>(
            std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() %
            1000) /
        1000.0f;

    // Seconds since the beginning of the current 12-hour period
    float clockTime = (localTime.tm_hour % 12) * 3600.0f + localTime.tm_min * 60.0f +
                      localTime.tm_sec + milliseconds;

    shaders_.at("clockShader").setUniform("timeSeconds", clockTime);
}

void Scene::draw(float aspectRatio)
{
    // Fixed draw order keeps the IDs stable between frames.
    nextDrawID_ = nextObjectID_;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera_.setAspectRatio(aspectRatio);

    if (currentScene_ == 3 || currentScene_ == 6 || currentScene_ == 11)
        updateClock();

    switch (currentScene_)
    {
    case 0:
        drawSkyDomeScene();
        break;
    case 1:
        drawTriangleScene();
        break;
    case 2:
        drawSphereScene();
        break;
    case 3:
        drawForestScene();
        break;
    case 4:
        drawLoginScene();
        break;
    case 5:
        drawSolarSystemScene();
        break;
    case 6:
        drawClockScene();
        break;
    case 7:
        drawLightingScene();
        break;
    case 8:
        drawTextureScene();
        break;
    case 9:
        drawSkyboxScene();
        break;
    case 10:
        drawCharactersScene();
        break;
    case 11:
        drawClassroomScene();
        break;
    case 12:
        drawTerrainScene();
        break;
    }

    if (supportsPlacement())
        firstPlacedID_[currentScene_] = nextDrawID_;
    drawPlacedObjects();
}

void Scene::drawTriangleScene()
{
    auto& shader = shaders_.at("basic");

    // TRIANGLE
    auto transformation = std::make_shared<CompositeTransformation>();

    transformation->add(std::make_shared<Translation>(glm::vec3(0.5f, 0.0f, 0.0f)));

    transformation->add(
        std::make_shared<Rotation>(glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

    DrawableObject triangle(&models_.at("triangle"), &shaders_.at("basic"));
    triangle.setTransformation(transformation);
    triangle.setID(allocateDrawID());

    shader.setUniform("fragmentColor", 1.0f, 1.0f, 1.0f);
    triangle.draw(time_);

    // LOGIN
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.5f, -1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, angleY_, glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.28f));

    shader.setUniform("fragmentColor", 1.0f, 1.0f, 1.0f);
    shader.setUniform("modelMatrix", modelMatrix);
    shader.use();
    drawModel("login");
    ShaderProgram::unuse();
}

void Scene::drawSphereScene()
{
    auto& shader = shaders_.at("basic");

    // SPHERE
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::rotate(modelMatrix, angleY_, glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.65f * scale_));

    shader.setUniform("fragmentColor", 1.0f, 1.0f, 1.0f);
    shader.setUniform("modelMatrix", modelMatrix);
    shader.use();
    drawModel("sphere");
    ShaderProgram::unuse();

    // LOGIN
    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, -1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, angleY_, glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.28f));

    shader.setUniform("fragmentColor", 1.0f, 1.0f, 1.0f);
    shader.setUniform("modelMatrix", modelMatrix);
    shader.use();
    drawModel("login");
    ShaderProgram::unuse();
}

void Scene::drawForestScene()
{
    auto& shader = shaders_.at("basic");
    auto& sunShader = shaders_.at("sunShader");
    auto& groundShader = shaders_.at("groundShader");
    auto& clockShader = shaders_.at("clockShader");

    firefly_.position =
        glm::vec3(1.8f * glm::sin(time_ * 0.65f), 0.45f + 0.20f * glm::sin(time_ * 1.6f),
                  -2.5f + 0.8f * glm::cos(time_ * 0.55f));
    // GROUND
    glm::mat4 groundM = glm::mat4(1.0f);
    groundM = glm::translate(groundM, glm::vec3(0.0f, -0.01f, -3.75f));
    groundM = glm::rotate(groundM, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    groundM = glm::scale(groundM, glm::vec3(7.0f, 9.0f, 1.0f));

    groundShader.setUniform("modelMatrix", groundM);
    firefly_.apply(groundShader);
    groundShader.use();
    drawModel("square");
    ShaderProgram::unuse();

    // TREES + BUSHES
    firefly_.apply(shaders_.at("treeShader"));

    for (const auto& treeObject : trees_)
        treeObject.draw(time_);

    firefly_.apply(shaders_.at("bushShader"));

    for (const auto& bushObject : bushes_)
        bushObject.draw(time_);

    // LOGIN
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.6f, 0.07f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.18f));

    shader.setUniform("fragmentColor", 1.0f, 1.0f, 1.0f);
    shader.setUniform("modelMatrix", modelMatrix);
    shader.use();
    drawModel("login");
    ShaderProgram::unuse();

    // CLOCK
    glm::mat4 clockM = glm::mat4(1.0f);
    clockM = glm::translate(clockM, glm::vec3(2.0f, 0.2f, 0.0f));
    clockM = glm::scale(clockM, glm::vec3(1.0f, 1.0f, -1.0f)); // clock without mirroring
    clockM = glm::scale(clockM, glm::vec3(0.2f * scale_));

    clockShader.setUniform("modelMatrix", clockM);

    clockShader.use();
    drawModel("clock");
    ShaderProgram::unuse();

    // CLOCK 2
    glm::mat4 clockM2 = glm::mat4(1.0f);
    clockM2 = glm::translate(clockM2, glm::vec3(-2.0f, 0.2f, 0.0f));
    clockM2 = glm::scale(clockM2, glm::vec3(1.0f, 1.0f, -1.0f)); // clock without mirroring
    clockM2 = glm::scale(clockM2, glm::vec3(0.2f * scale_));

    clockShader.setUniform("modelMatrix", clockM2);

    clockShader.use();
    drawModel("clock");
    ShaderProgram::unuse();

    // FIREFLY

    glm::mat4 sunM = glm::mat4(1.0f);
    sunM = glm::translate(sunM, firefly_.position);
    sunM = glm::scale(sunM, glm::vec3(0.02f));

    sunShader.setUniform("modelMatrix", sunM);
    sunShader.use();
    drawModel("sphere");
    ShaderProgram::unuse();
}

void Scene::drawLoginScene()
{
    auto& shader = shaders_.at("basic");

    // LOGIN
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::rotate(modelMatrix, angleY_, glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(scale_));

    shader.setUniform("fragmentColor", 1.0f, 1.0f, 1.0f);
    shader.setUniform("modelMatrix", modelMatrix);
    shader.use();
    drawModel("login");
    ShaderProgram::unuse();
}

void Scene::drawSolarSystemScene()
{
    auto& shader = shaders_.at("basic");
    auto& ambientShader = shaders_.at("ambientShader");
    auto& phongShader = shaders_.at("PhongShader");
    auto& blinnShader = shaders_.at("blinnShader");

    /*SOLAR SYSTEM */

    // SUN - AMBIENT
    ambientShader.setUniform("useModelColor", 1.0f);

    // EARTH - PHONG
    phongShader.setUniform("useModelColor", 1.0f);
    phongShader.setUniform("useSphericalNormal", 1.0f);
    phongShader.setUniform("useFlatNormal", 0.0f);
    phongShader.setUniform("lightPosition", 0.0f, 0.0f, 0.0f);

    // MOON - BLINN
    blinnShader.setUniform("useModelColor", 1.0f);
    blinnShader.setUniform("useSphericalNormal", 1.0f);
    blinnShader.setUniform("useFlatNormal", 0.0f);
    blinnShader.setUniform("lightPosition", 0.0f, 0.0f, 0.0f);

    for (const auto& object : solarSystem_) // drawing every object in solar system
    {
        object.draw(time_);
    }

    // ILLUMINATI
    glm::mat4 illuminatiM = glm::mat4(1.0f);
    illuminatiM = glm::translate(illuminatiM, glm::vec3(-1.34f, 0.12f, 0.0f));
    illuminatiM = glm::rotate(illuminatiM, time_ * 0.75f, glm::vec3(0.0f, 1.0f, 0.0f));
    illuminatiM = glm::scale(illuminatiM, glm::vec3(0.13f));

    blinnShader.setUniform("useModelColor", 1.0f);
    blinnShader.setUniform("useSphericalNormal", 0.0f);
    blinnShader.setUniform("useFlatNormal", 1.0f);
    blinnShader.setUniform("lightPosition", 0.0f, 0.0f, 0.0f);
    blinnShader.setUniform("modelMatrix", illuminatiM);

    blinnShader.use();
    drawModel("illuminati");
    ShaderProgram::unuse();

    // ALIEN
    glm::mat4 alienM = glm::mat4(1.0f);
    alienM = glm::rotate(alienM, time_ * 0.36f, glm::vec3(0.0f, 1.0f, 0.0f)); // Orbit around Sun
    alienM = glm::translate(alienM, glm::vec3(1.05f, 0.0f, 0.0f));
    alienM =
        glm::rotate(alienM, time_ * 0.65f, glm::vec3(0.0f, 1.0f, 0.0f)); // Spin around own axis
    alienM = glm::scale(alienM, glm::vec3(0.08f));

    phongShader.setUniform("useModelColor", 1.0f);
    phongShader.setUniform("useSphericalNormal", 0.0f);
    phongShader.setUniform("useFlatNormal", 1.0f);
    phongShader.setUniform("lightPosition", 0.0f, 0.0f, 0.0f);
    phongShader.setUniform("modelMatrix", alienM);

    phongShader.use();
    drawModel("alien");
    ShaderProgram::unuse();

    // LOGIN
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-1.24f, -0.65f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, angleY_, glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.20f));

    shader.setUniform("modelMatrix", modelMatrix);
    shader.use();
    drawModel("login");
    ShaderProgram::unuse();
}

void Scene::drawClockScene()
{
    auto& shader = shaders_.at("basic");
    auto& clockShader = shaders_.at("clockShader");

    // CLOCK only
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, -1.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.7f * scale_));

    clockShader.setUniform("modelMatrix", modelMatrix);

    clockShader.use();
    drawModel("clock"); // by 1 draw call
    ShaderProgram::unuse();

    // LOGIN
    glm::mat4 modelMatrix2 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.24f, -0.65f, 0.0f));
    modelMatrix2 = glm::rotate(modelMatrix2, angleY_, glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(0.20f));

    shader.setUniform("modelMatrix", modelMatrix2);
    shader.use();
    drawModel("login");
    ShaderProgram::unuse();
}

void Scene::drawLightingScene()
{
    auto& shader = shaders_.at("basic");
    auto& ambientShader = shaders_.at("ambientShader");
    auto& lambertShader = shaders_.at("LambertShader");
    auto& phongShader = shaders_.at("PhongShader");
    auto& blinnShader = shaders_.at("blinnShader");

    // 4 SPHERES
    glm::mat4 modelMatrix1 = glm::mat4(1.0f);
    modelMatrix1 = glm::translate(modelMatrix1, glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix1 = glm::scale(modelMatrix1, glm::vec3(0.45f * scale_));
    ambientShader.setUniform("useModelColor", 0.0f);
    ambientShader.setUniform("modelMatrix", modelMatrix1);
    ambientShader.use();
    drawModel("sphere");
    ShaderProgram::unuse();

    glm::mat4 modelMatrix2 = glm::mat4(1.0f);
    modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0.0f, -1.0f, 0.0f));
    modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(0.45f * scale_));
    blinnShader.setUniform("useModelColor", 0.0f);
    blinnShader.setUniform("useSphericalNormal", 1.0f);
    blinnShader.setUniform("useFlatNormal", 0.0f);
    blinnShader.setUniform("lightPosition", 0.0f, 0.0f, 0.0f);
    blinnShader.setUniform("modelMatrix", modelMatrix2);
    blinnShader.use();
    drawModel("sphere");
    ShaderProgram::unuse();

    glm::mat4 modelMatrix3 = glm::mat4(1.0f);
    modelMatrix3 = glm::translate(modelMatrix3, glm::vec3(-1.0f, 0.0f, 0.0f));
    modelMatrix3 =
        glm::rotate(modelMatrix3, glm::radians(60.0f), glm::vec3(0.0f, -1.0f, 0.0f)); // for problem
    modelMatrix3 = glm::scale(modelMatrix3, glm::vec3(0.45f * scale_));
    lambertShader.setUniform("useModelColor", 0.0f);
    lambertShader.setUniform("lightPosition", 0.0f, 0.0f, 0.0f);
    lambertShader.setUniform("modelMatrix", modelMatrix3);
    lambertShader.use();
    drawModel("sphere");
    ShaderProgram::unuse();

    glm::mat4 modelMatrix4 = glm::mat4(1.0f);
    modelMatrix4 = glm::translate(modelMatrix4, glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix4 = glm::scale(modelMatrix4, glm::vec3(0.45f * scale_));
    phongShader.setUniform("useModelColor", 0.0f);
    phongShader.setUniform("useSphericalNormal", 1.0f);
    phongShader.setUniform("useFlatNormal", 0.0f);
    phongShader.setUniform("lightPosition", 0.0f, 0.0f, 0.0f);
    phongShader.setUniform("modelMatrix", modelMatrix4);

    phongShader.use();
    drawModel("sphere");
    ShaderProgram::unuse();

    // LOGIN
    glm::mat4 modelMatrix5 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.24f, -0.65f, 0.0f));
    modelMatrix5 = glm::rotate(modelMatrix5, angleY_, glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix5 = glm::scale(modelMatrix5, glm::vec3(0.20f));

    shader.setUniform("modelMatrix", modelMatrix5);
    shader.use();
    drawModel("login");
    ShaderProgram::unuse();
}

void Scene::drawTextureScene()
{

    ShaderProgram& textureShader = shaders_.at("textureShader");

    // WOOD
    glm::mat4 modelMatrix1 = glm::mat4(1.0f);
    modelMatrix1 = glm::translate(modelMatrix1, glm::vec3(-1.0f, 0.0f, -1.0f));
    modelMatrix1 = glm::rotate(modelMatrix1, glm::radians(65.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix1 = glm::scale(modelMatrix1, glm::vec3(0.7f));

    textureShader.setUniform("modelMatrix", modelMatrix1);
    textureShader.setUniform("textureUnitID", 0);

    woodTexture_->bind(0);

    textureShader.use();
    drawModel("plainUV");
    ShaderProgram::unuse();

    // GRASS
    glm::mat4 modelMatrix2 = glm::mat4(1.0f);
    modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(1.0f, 0.0f, -1.0f));
    modelMatrix2 = glm::rotate(modelMatrix2, glm::radians(65.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(0.7f));

    textureShader.setUniform("modelMatrix", modelMatrix2);
    textureShader.setUniform("textureUnitID", 1);

    grassTexture_->bind(1);

    textureShader.use();
    drawModel("plainUV");
    ShaderProgram::unuse();

    // MIXED TEXTURES
    glm::mat4 modelMatrix3 = glm::mat4(1.0f);

    modelMatrix3 = glm::translate(modelMatrix3, glm::vec3(0.0f, -1.15f, -1.0f));
    modelMatrix3 = glm::rotate(modelMatrix3, glm::radians(65.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix3 = glm::scale(modelMatrix3, glm::vec3(0.65f));

    ShaderProgram& mixShader = shaders_.at("mixShader");

    mixShader.setUniform("modelMatrix", modelMatrix3);
    mixShader.setUniform("woodTexture", 0);
    mixShader.setUniform("grassTexture", 1);

    woodTexture_->bind(0);
    grassTexture_->bind(1);

    mixShader.use();
    drawModel("plainUV");
    ShaderProgram::unuse();
}

void Scene::drawSkyboxScene()
{

    drawSkybox();
}

void Scene::drawSkyDomeScene()
{

    // GROUND
    ShaderProgram& textureShader = shaders_.at("textureShader");

    glm::mat4 groundM = glm::mat4(1.0f);
    groundM = glm::translate(groundM, glm::vec3(0.0f, -0.5f, 0.0f));
    groundM = glm::scale(groundM, glm::vec3(45.0f, 1.0f, 45.0f));

    textureShader.setUniform("modelMatrix", groundM);
    textureShader.setUniform("textureUnitID", 0);

    grassTexture_->bind(0);

    textureShader.use();
    drawModel("plainUV");
    ShaderProgram::unuse();

    // SKYDOME
    ShaderProgram& skyDomeShader = shaders_.at("skyDomeShader");

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    skyDomeShader.setUniform("skyTexture", 0);

    skybox_->bind(0);

    skyDomeShader.use();
    drawModel("skyDome");
    ShaderProgram::unuse();

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

void Scene::drawCharactersScene()
{

    auto& textureShader = shaders_.at("textureShader");
    auto ground = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.7f, -2.0f));
    ground = glm::scale(ground, glm::vec3(5.0f, 1.0f, 5.0f));

    textureShader.setUniform("modelMatrix", ground);
    textureShader.setUniform("textureUnitID", 0);
    grassTexture_->bind(0);
    textureShader.use();
    drawModel("plainUV");

    // Position (X, Y, Z), rotation in degrees (X, Y, Z), scale (X, Y, Z).
    drawTexturedModel("shrek",
        glm::vec3(-1.65f, -0.69f, -2.0f), // position (X, Y, Z)
        glm::vec3(0.0f, 0.0f, 0.0f),      // rotation (degrees: X, Y, Z)
        glm::vec3(1.0f, 1.0f, 1.0f)       // scale (X, Y, Z)
    );

    drawTexturedModel("toiled",
        glm::vec3(0.0f, -0.69f, -2.0f), // position (X, Y, Z)
        glm::vec3(0.0f, 0.0f, 0.0f),    // rotation (degrees: X, Y, Z)
        glm::vec3(1.0f, 1.0f, 1.0f)     // scale (X, Y, Z)
    );

    drawTexturedModel("fiona",
        glm::vec3(1.65f, -0.69f, -2.0f),      // position (X, Y, Z)
        glm::vec3(0.0f, time_ * 45.0f, 0.0f), // rotation (degrees: X, Y, Z)
        glm::vec3(1.0f, 1.0f, 1.0f)           // scale (X, Y, Z)
    );

    drawSkybox();
}

void Scene::drawClassroomScene()
{
    auto& clockShader = shaders_.at("clockShader");

    drawSkybox();

    drawTexturedModel("classroom",
        glm::vec3(0.0f, -0.7f, 0.0f), // position (X, Y, Z)
        glm::vec3(0.0f),              // rotation (degrees: X, Y, Z)
        glm::vec3(1.0f),              // scale (X, Y, Z)
        "classroom"                   // texture
    );

    drawTexturedModel("door",
        glm::vec3(-3.0f, -0.7f, 3.75f), // position (X, Y, Z)
        glm::vec3(0.0f, 0.0f, 0.0f),    // rotation (degrees: X, Y, Z)
        glm::vec3(0.95f, 1.0f, 1.0f)    // scale (X, Y, Z)
    );

    drawTexturedModel("whiteboard",
        glm::vec3(-3.79f, 0.35f, 1.0f), // position (X, Y, Z)
        glm::vec3(0.0f),                // rotation (degrees: X, Y, Z)
        glm::vec3(0.90f)                // scale (X, Y, Z)
    );

    // CLOCK
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    modelMatrix = glm::translate(modelMatrix, glm::vec3(-3.75f, 1.85f, 1.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, -1.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));

    clockShader.setUniform("modelMatrix", modelMatrix);

    clockShader.use();
    drawModel("clock");
    ShaderProgram::unuse();

    auto drawClassroomBox =
        [&](const glm::vec3& position, const glm::vec3& size, const glm::vec3& color)
    {
        auto transform = glm::translate(glm::mat4(1.0f), position);
        transform = glm::scale(transform, size);
        auto& boxShader = shaders_.at("classroomShader");
        boxShader.setUniform("modelMatrix", transform);
        boxShader.setUniform("baseColor", color.r, color.g, color.b);
        boxShader.use();
        drawModel("classroomBox");
        ShaderProgram::unuse();
    };

    for (int row = 0; row < 3; ++row)
    {
        const float rowX = -1.95f + row * 1.95f;
        for (int column = 0; column < 3; ++column)
        {
            const float columnZ = (column - 1) * 2.0f;
            drawTexturedModel("table",
                glm::vec3(rowX, -0.7f, columnZ), // position (X, Y, Z)
                glm::vec3(0.0f, 90.0f, 0.0f),    // rotation (degrees: X, Y, Z)
                glm::vec3(0.90f)                 // scale (X, Y, Z)
            );
            drawTexturedModel("orangeChair",
                glm::vec3(rowX + 0.78f, -0.7f, columnZ), // position (X, Y, Z)
                glm::vec3(0.0f, -90.0f, 0.0f),           // rotation (degrees: X, Y, Z)
                glm::vec3(0.88f)                         // scale (X, Y, Z)
            );

            const float tabletopY = -0.7f + 0.90f * 0.66f;
            // Laptop base
            drawClassroomBox(
                glm::vec3(rowX, tabletopY + 0.020f, columnZ), // position (X, Y, Z)
                glm::vec3(0.40f, 0.040f, 0.56f),              // scale (size along X, Y, Z)
                glm::vec3(0.54f, 0.56f, 0.55f)                // color (R, G, B)
            );
            // Keyboard
            drawClassroomBox(
                glm::vec3(rowX + 0.035f, tabletopY + 0.043f, columnZ), // position (X, Y, Z)
                glm::vec3(0.27f, 0.006f, 0.43f),                       // scale (size along X, Y, Z)
                glm::vec3(0.12f, 0.13f, 0.14f)                         // color (R, G, B)
            );
            // Touchpad
            drawClassroomBox(
                glm::vec3(rowX + 0.135f, tabletopY + 0.043f, columnZ), // position (X, Y, Z)
                glm::vec3(0.075f, 0.007f, 0.13f),                      // scale (size along X, Y, Z)
                glm::vec3(0.68f, 0.69f, 0.67f)                         // color (R, G, B)
            );
            // Display frame
            drawClassroomBox(
                glm::vec3(rowX - 0.175f, tabletopY + 0.205f, columnZ), // position (X, Y, Z)
                glm::vec3(0.028f, 0.33f, 0.56f),                       // scale (size along X, Y, Z)
                glm::vec3(0.09f, 0.10f, 0.11f)                         // color (R, G, B)
            );
            // Screen
            drawClassroomBox(
                glm::vec3(rowX - 0.157f, tabletopY + 0.205f, columnZ), // position (X, Y, Z)
                glm::vec3(0.008f, 0.275f, 0.50f),                      // scale (size along X, Y, Z)
                glm::vec3(0.10f, 0.30f, 0.42f)                         // color (R, G, B)
            );
        }
    }
}

void Scene::drawTerrainScene()
{

    drawSkybox();
    drawTexturedModel("teren",
        glm::vec3(0.0f, -0.7f, 0.0f), // position (X, Y, Z)
        glm::vec3(0.0f),              // rotation (degrees: X, Y, Z)
        glm::vec3(1.0f),              // scale (X, Y, Z)
        "teren"                       // texture
    );
}
