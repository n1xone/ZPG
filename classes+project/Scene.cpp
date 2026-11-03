#include "Scene.h"

#include <glm/glm.hpp> //for my matrix
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>
#include <random>
#include <chrono>
#include <ctime>
#include "DrawableObject.h"
#include "Transformation.h"

#include "models/sphere.h"
#include "models/OpenGL.h" 
#include "models/CHE0086.h" //made with blender + GPT
#include "models/alien.h" //made with GPT
#include "models/illuminati.h" //made with GPT
#include "models/tree.h"
#include "models/bushes.h"

#include "models/earth.h"
#include "models/moon.h"
#include "models/sun.h"

#include "models/clock.h"

namespace {  //created by Nikita Chervinskyi 
    float triangle[] = {
        0.0f,  0.666f, 0.0f,  1.0f, 0.0f, 0.0f,
        0.5f, -0.333f, 0.0f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.333f, 0.0f,  0.0f, 0.0f, 1.0f
    };

    float square[] = {
        -0.6f,  0.6f, 0.0f,   0.0f, 1.0f, 0.0f,
        0.6f,  0.6f, 0.0f,   0.0f, 1.0f, 0.0f,
        0.6f, -0.6f, 0.0f,   0.0f, 1.0f, 0.0f,

        -0.6f,  0.6f, 0.0f,   0.0f, 1.0f, 0.0f,
        -0.6f, -0.6f, 0.0f,   0.0f, 1.0f, 0.0f,
        0.6f, -0.6f, 0.0f,   0.0f, 1.0f, 0.0f
    };
}

Scene::Scene()
{
    models_.try_emplace("triangle", triangle, sizeof(triangle));
    models_.try_emplace("sphere", sphere, sizeof(sphere));
    models_.try_emplace("square", square, sizeof(square));
    models_.try_emplace("login", che0086, sizeof(che0086));
    models_.try_emplace("openGL", opengl, sizeof(opengl));
    models_.try_emplace("tree", tree, sizeof(tree));
    models_.try_emplace("bushes", bushes, sizeof(bushes));
    models_.try_emplace("sun", sun, sizeof(sun), 9);
    models_.try_emplace("earth", earth, sizeof(earth), 9);
    models_.try_emplace("moon", moon, sizeof(moon), 9);
    models_.try_emplace("clock", clockModel, sizeof(clockModel), 9, 6);
    models_.try_emplace("alien", alien, sizeof(alien), 9);
    models_.try_emplace("illuminati", illuminati, sizeof(illuminati), 9);

    shaders_.try_emplace("basic", "shaders/Basic.vert", "shaders/Basic.frag");
    shaders_.try_emplace("treeShader", "shaders/Tree.vert", "shaders/Tree.frag");
    shaders_.try_emplace("bushShader", "shaders/Bush.vert", "shaders/Bush.frag");
    shaders_.try_emplace("sunShader", "shaders/Sun.vert", "shaders/Sun.frag");
    shaders_.try_emplace("groundShader", "shaders/Ground.vert", "shaders/Ground.frag");
    shaders_.try_emplace("clockShader", "shaders/Clock.vert", "shaders/Clock.frag");
    shaders_.try_emplace("ambientShader", "shaders/Ambient.vert", "shaders/Ambient.frag");
    shaders_.try_emplace("blinnShader", "shaders/Blinn.vert", "shaders/Blinn.frag");
    shaders_.try_emplace("PhongShader", "shaders/Phong.vert", "shaders/Phong.frag");
    shaders_.try_emplace("LambertShader", "shaders/Lambert.vert", "shaders/Lambert.frag");

    for (auto& entry : shaders_) // enable my observer
    {
        camera_.attachShader(&entry.second);
    }


    // FOREST
    std::mt19937 generator(42);

    std::uniform_real_distribution<float> randomX(-4.0f, 4.0f);
    std::uniform_real_distribution<float> randomZ(-9.0f, 1.5f);
    std::uniform_real_distribution<float> randomRotation(
        0.0f, glm::radians(360.0f)
    );

    std::uniform_real_distribution<float> treeScale(0.05f, 0.19f);
    std::uniform_real_distribution<float> bushScale(0.10f, 0.20f);

    auto createForestObjects = [&](const char* modelName,
                                   const char* shaderName,
                                   auto& randomScale,
                                   auto& objects)
    {
        for (int i = 0; i < 74; ++i)
        {
            DrawableObject object(
                &models_.at(modelName),
                &shaders_.at(shaderName)
            );

            auto transformation = std::make_shared<CompositeTransformation>();

            transformation->add(std::make_shared<Translation>(
                glm::vec3(randomX(generator), 0.0f, randomZ(generator))
            ));

            transformation->add(std::make_shared<Rotation>(
                randomRotation(generator),
                glm::vec3(0.0f, 1.0f, 0.0f)
            ));

            transformation->add(std::make_shared<Scale>(
                glm::vec3(randomScale(generator))
            ));

            object.setTransformation(transformation);
            objects.push_back(object);
        }
    };

    createForestObjects("tree", "treeShader", treeScale, trees_);
    createForestObjects("bushes", "bushShader", bushScale, bushes_);



    // SOLAR SYSTEM
    const glm::vec3 yAxis(0.0f, 1.0f, 0.0f);

    auto systemTransform = std::make_shared<CompositeTransformation>();
    systemTransform->add(std::make_shared<Rotation>(
        glm::radians(15.0f),
        glm::vec3(1.0f, 0.0f, 0.0f)
    ));

    // Common function for creating solar objects
    auto createSolarBody = [&](const char* modelName,
                               const char* shaderName,
                               std::shared_ptr<CompositeTransformation> parent,
                               float rotationSpeed,
                               float size)
    {
        DrawableObject object(
            &models_.at(modelName),
            &shaders_.at(shaderName)
        );

        auto transformation = std::make_shared<CompositeTransformation>();

        transformation->add(parent);
        transformation->add(std::make_shared<Rotation>(
            0.0f, yAxis, rotationSpeed
        ));
        transformation->add(std::make_shared<Scale>(glm::vec3(size)));

        object.setTransformation(transformation);
        solarSystem_.push_back(object);
    };

    // SUN
    createSolarBody("sun", "ambientShader", systemTransform, 0.4f, 0.22f);

    // EARTH ORBIT
    auto earthPosition = std::make_shared<CompositeTransformation>();
    earthPosition->add(systemTransform);
    earthPosition->add(std::make_shared<Rotation>(
        0.0f, yAxis, 0.5f
    ));
    earthPosition->add(std::make_shared<Translation>(
        glm::vec3(0.60f, 0.0f, 0.0f)
    ));

    // EARTH
    createSolarBody("earth", "PhongShader", earthPosition, 1.5f, 0.10f);

    // MOON ORBIT AROUND EARTH
    auto moonPosition = std::make_shared<CompositeTransformation>();
    moonPosition->add(earthPosition);
    moonPosition->add(std::make_shared<Rotation>(
        0.0f, yAxis, 2.0f
    ));
    moonPosition->add(std::make_shared<Translation>(
        glm::vec3(0.19f, 0.0f, 0.0f)
    ));

    // MOON
    createSolarBody("moon", "blinnShader", moonPosition, 1.0f, 0.04f);
}
void Scene::setScene(int scene)
{
    if (currentScene_ == scene)
        return;

    currentScene_ = scene;
    scale_ = 1.0f;
}

void Scene::update(float movementX, float movementY, float scaleChange, float time, float mouseX, float mouseY)
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

void Scene::draw(float aspectRatio)
{
    ShaderProgram& shader = shaders_.at("basic");
    ShaderProgram& sunShader = shaders_.at("sunShader");
    ShaderProgram& groundShader = shaders_.at("groundShader");
    ShaderProgram& clockShader = shaders_.at("clockShader");
    ShaderProgram& ambientShader = shaders_.at("ambientShader");
    ShaderProgram& LambertShader = shaders_.at("LambertShader");
    ShaderProgram& PhongShader = shaders_.at("PhongShader");
    ShaderProgram& blinnShader = shaders_.at("blinnShader");


    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    

    /* made a current time with ai */
    // Current local system time
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm localTime = *std::localtime(&currentTime);

    // Milliseconds for smooth movement
    float milliseconds = static_cast<float>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()
        ).count() % 1000
    ) / 1000.0f;

    // Seconds since the beginning of the current 12-hour period
    float clockTime =
        (localTime.tm_hour % 12) * 3600.0f +
        localTime.tm_min * 60.0f +
        localTime.tm_sec +
        milliseconds;

    clockShader.setUniform("timeSeconds", clockTime);
    
    camera_.setAspectRatio(aspectRatio); //observer


/*--------------------------------------MY SCENES---------------------------------------------*/
    if (currentScene_ == 1)
    {
        //TRIANGLE
        auto transformation = std::make_shared<CompositeTransformation>();

        transformation->add(std::make_shared<Translation>(
            glm::vec3(0.5f, 0.0f, 0.0f)
        ));

        transformation->add(std::make_shared<Rotation>(
            glm::radians(45.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)
        ));

        DrawableObject triangle(
            &models_.at("triangle"),
            &shaders_.at("basic")
        );
        triangle.setTransformation(transformation);

        shader.setUniform("fragmentColor", 1.0f, 1.0f, 1.0f);
        triangle.draw(time_);

        // LOGIN
        glm::mat4 M = glm::mat4(1.0f);
        M = glm::translate(M, glm::vec3(0.5f, -1.0f, 0.0f));
        M = glm::rotate(M, angleY_, glm::vec3(0.0f, 1.0f, 0.0f));
        M = glm::scale(M, glm::vec3(0.28f));

        shader.setUniform("fragmentColor", 1.0f, 1.0f, 1.0f);
        shader.setUniform("modelMatrix", M);
        shader.use();
        models_.at("login").draw();
        ShaderProgram::unuse();
    }
    else if (currentScene_ == 2)
    {
        //SPHERE
        glm::mat4 M = glm::mat4(1.0f);
        M = glm::translate(M, glm::vec3(0.0f, 0.0f, 0.0f));
        M = glm::rotate(M, angleY_, glm::vec3(0.0f, 1.0f, 0.0f));
        M = glm::scale(M, glm::vec3(0.65f * scale_));

        shader.setUniform("fragmentColor", 1.0f, 1.0f, 1.0f);
        shader.setUniform("modelMatrix", M);
        shader.use();
        models_.at("sphere").draw();
        ShaderProgram::unuse();

        // LOGIN
        M = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, -1.0f, 0.0f));
        M = glm::rotate(M, angleY_, glm::vec3(0.0f, 1.0f, 0.0f));
        M = glm::scale(M, glm::vec3(0.28f));

        shader.setUniform("fragmentColor", 1.0f, 1.0f, 1.0f);
        shader.setUniform("modelMatrix", M);
        shader.use();
        models_.at("login").draw();
        ShaderProgram::unuse();
    }
    else if (currentScene_ == 3)
    {        
        firefly_.position = glm::vec3(
            1.8f * glm::sin(time_ * 0.65f),
            0.45f + 0.20f * glm::sin(time_ * 1.6f),
            -2.5f + 0.8f * glm::cos(time_ * 0.55f)
        );
        // GROUND
        glm::mat4 groundM = glm::mat4(1.0f);
        groundM = glm::translate(groundM, glm::vec3(0.0f, -0.01f, -3.75f));
        groundM = glm::rotate(groundM, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        groundM = glm::scale(groundM, glm::vec3(7.0f, 9.0f, 1.0f));

        groundShader.setUniform("modelMatrix", groundM);
        firefly_.apply(groundShader);
        groundShader.use();
        models_.at("square").draw();
        ShaderProgram::unuse();

        //TREES + BUSHES
        firefly_.apply(shaders_.at("treeShader"));

        for (const auto& treeObject : trees_)
            treeObject.draw(time_);

        firefly_.apply(shaders_.at("bushShader"));

        for (const auto& bushObject : bushes_)
            bushObject.draw(time_);

        
        // LOGIN
        glm::mat4 M = glm::translate(glm::mat4(1.0f), glm::vec3(1.6f, 0.07f, 0.0f));
        //M = glm::rotate(M, angleY_, glm::vec3(0.0f, 1.0f, 0.0f)); //without rotation
        M = glm::scale(M, glm::vec3(0.18f));

        shader.setUniform("fragmentColor", 1.0f, 1.0f, 1.0f);
        shader.setUniform("modelMatrix", M);
        shader.use();
        models_.at("login").draw();
        ShaderProgram::unuse();
        
        //CLOCK
        glm::mat4 clockM = glm::mat4(1.0f);
        clockM = glm::translate(clockM, glm::vec3(2.0f, 0.2f, 0.0f));
        clockM = glm::scale(clockM, glm::vec3(1.0f, 1.0f, -1.0f));//clock without mirroring 
        clockM = glm::scale(clockM, glm::vec3(0.2f * scale_));

        clockShader.setUniform("modelMatrix", clockM);

        clockShader.use();
        models_.at("clock").draw();
        ShaderProgram::unuse();


        //CLOCK 2
        glm::mat4 clockM2 = glm::mat4(1.0f);
        clockM2 = glm::translate(clockM2, glm::vec3(-2.0f, 0.2f, 0.0f));
        clockM2 = glm::scale(clockM2, glm::vec3(1.0f, 1.0f, -1.0f));//clock without mirroring 
        clockM2 = glm::scale(clockM2, glm::vec3(0.2f * scale_));

        clockShader.setUniform("modelMatrix", clockM2);

        clockShader.use();
        models_.at("clock").draw();
        ShaderProgram::unuse();

        // FIREFLY

        glm::mat4 sunM = glm::mat4(1.0f);
        sunM = glm::translate(sunM, firefly_.position);
        sunM = glm::scale(sunM, glm::vec3(0.02f));

        sunShader.setUniform("modelMatrix", sunM);
        sunShader.use();
        models_.at("sphere").draw();
        ShaderProgram::unuse();
    }
    else if (currentScene_ == 4)
    {
        //LOGIN
        glm::mat4 M = glm::mat4(1.0f);
        M = glm::translate(M, glm::vec3(0.0f, 0.0f, 0.0f));
        M = glm::rotate(M, angleY_, glm::vec3(0.0f, 1.0f, 0.0f));
        M = glm::scale(M, glm::vec3(scale_));

        shader.setUniform("fragmentColor", 1.0f, 1.0f, 1.0f);
        shader.setUniform("modelMatrix", M);
        shader.use();
        models_.at("login").draw();
        ShaderProgram::unuse();
    }
    else if (currentScene_ == 5)
    {
        /*SOLAR SYSTEM */

        // SUN - AMBIENT
        ambientShader.setUniform("useModelColor", 1.0f);

        // EARTH - PHONG
        PhongShader.setUniform("useModelColor", 1.0f);
        PhongShader.setUniform("useSphericalNormal", 1.0f);
        PhongShader.setUniform("useFlatNormal", 0.0f);
        PhongShader.setUniform("lightPosition", 0.0f, 0.0f, 0.0f);

        // MOON - BLINN
        blinnShader.setUniform("useModelColor", 1.0f);
        blinnShader.setUniform("useSphericalNormal", 1.0f);
        blinnShader.setUniform("useFlatNormal", 0.0f);
        blinnShader.setUniform("lightPosition", 0.0f, 0.0f, 0.0f);

        for (const auto& object : solarSystem_) //drawing every object in solar system
        {
            object.draw(time_);
        }

        // ILLUMINATI
        glm::mat4 illuminatiM = glm::mat4(1.0f);
        illuminatiM = glm::translate(illuminatiM, glm::vec3(-1.34f, 0.12f, 0.0f));
        illuminatiM = glm::rotate(illuminatiM,time_ * 0.75f,glm::vec3(0.0f, 1.0f, 0.0f));
        illuminatiM = glm::scale(illuminatiM, glm::vec3(0.13f));

        blinnShader.setUniform("useModelColor", 1.0f);
        blinnShader.setUniform("useSphericalNormal", 0.0f);
        blinnShader.setUniform("useFlatNormal", 1.0f);
        blinnShader.setUniform("lightPosition", 0.0f, 0.0f, 0.0f);
        blinnShader.setUniform("modelMatrix", illuminatiM);

        blinnShader.use();
        models_.at("illuminati").draw();
        ShaderProgram::unuse();

        // ALIEN
        glm::mat4 alienM = glm::mat4(1.0f);
        alienM = glm::rotate(alienM,time_ * 0.36f, glm::vec3(0.0f, 1.0f, 0.0f));// Orbit around Sun
        alienM = glm::translate(alienM, glm::vec3(1.05f, 0.0f, 0.0f));
        alienM = glm::rotate(alienM, time_ * 0.65f, glm::vec3(0.0f, 1.0f, 0.0f));  // Spin around own axis
        alienM = glm::scale(alienM, glm::vec3(0.08f));

        PhongShader.setUniform("useModelColor", 1.0f);
        PhongShader.setUniform("useSphericalNormal", 0.0f);
        PhongShader.setUniform("useFlatNormal", 1.0f);
        PhongShader.setUniform("lightPosition", 0.0f, 0.0f, 0.0f);
        PhongShader.setUniform("modelMatrix", alienM);

        PhongShader.use();
        models_.at("alien").draw();
        ShaderProgram::unuse();

        // LOGIN 
        glm::mat4 M = glm::translate(glm::mat4(1.0f), glm::vec3(-1.24f, -0.65f, 0.0f));
        M = glm::rotate(M, angleY_, glm::vec3(0.0f, 1.0f, 0.0f));
        M = glm::scale(M, glm::vec3(0.20f));

        shader.setUniform("modelMatrix", M);
        shader.use();
        models_.at("login").draw();
        ShaderProgram::unuse();
    }
    else if (currentScene_ == 6)
    {
        //CLOCK only
        glm::mat4 M = glm::mat4(1.0f);
        M = glm::translate(M, glm::vec3(0.0f, 0.0f, 0.0f));
        M = glm::scale(M, glm::vec3(1.0f, 1.0f, -1.0f));
        M = glm::scale(M, glm::vec3(0.7f * scale_));

        clockShader.setUniform("modelMatrix", M);

        clockShader.use();
        models_.at("clock").draw(); //by 1 draw call
        ShaderProgram::unuse();
    
        //LOGIN
        glm::mat4 M2 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.24f, -0.65f, 0.0f));
        M2 = glm::rotate(M2, angleY_, glm::vec3(0.0f, 1.0f, 0.0f));
        M2 = glm::scale(M2, glm::vec3(0.20f));

        shader.setUniform("modelMatrix", M2);
        shader.use();
        models_.at("login").draw();
        ShaderProgram::unuse();
    }
    else if (currentScene_ == 7)
    {
        //4 SPHERES
        glm::mat4 M1 = glm::mat4(1.0f);
        M1 = glm::translate(M1, glm::vec3(0.0f, 1.0f, 0.0f));
        M1 = glm::scale(M1, glm::vec3(0.45f * scale_));
        ambientShader.setUniform("useModelColor", 0.0f);
        ambientShader.setUniform("modelMatrix", M1);
        ambientShader.use();
        models_.at("sphere").draw();
        ShaderProgram::unuse();

        glm::mat4 M2 = glm::mat4(1.0f);
        M2 = glm::translate(M2, glm::vec3(0.0f, -1.0f, 0.0f));
        M2 = glm::scale(M2, glm::vec3(0.45f * scale_));
        blinnShader.setUniform("useModelColor", 0.0f);
        blinnShader.setUniform("useSphericalNormal", 1.0f);
        blinnShader.setUniform("useFlatNormal", 0.0f);
        blinnShader.setUniform("lightPosition", 0.0f, 0.0f, 0.0f);
        blinnShader.setUniform("modelMatrix", M2);
        blinnShader.use();
        models_.at("sphere").draw();
        ShaderProgram::unuse();

        glm::mat4 M3 = glm::mat4(1.0f);
        M3 = glm::translate(M3, glm::vec3(-1.0f, 0.0f, 0.0f));
        M3 = glm::rotate(M3, glm::radians(60.0f), glm::vec3(0.0f, -1.0f, 0.0f)); //for problem
        M3 = glm::scale(M3, glm::vec3(0.45f * scale_));
        LambertShader.setUniform("useModelColor", 0.0f);
        LambertShader.setUniform("lightPosition", 0.0f, 0.0f, 0.0f);
        LambertShader.setUniform("modelMatrix", M3);
        LambertShader.use();
        models_.at("sphere").draw();
        ShaderProgram::unuse();

        glm::mat4 M4 = glm::mat4(1.0f);
        M4 = glm::translate(M4, glm::vec3(1.0f, 0.0f, 0.0f));
        M4 = glm::scale(M4, glm::vec3(0.45f * scale_));
        PhongShader.setUniform("useModelColor", 0.0f);
        PhongShader.setUniform("useSphericalNormal", 1.0f);
        PhongShader.setUniform("useFlatNormal", 0.0f);
        PhongShader.setUniform("lightPosition", 0.0f, 0.0f, 0.0f);
        //PhongShader.setUniform("lightPosition", 1.0f, 0.0f, -5.0f);
        PhongShader.setUniform("modelMatrix", M4);

        PhongShader.use();
        models_.at("sphere").draw();
        ShaderProgram::unuse();

        //LOGIN
        glm::mat4 M5 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.24f, -0.65f, 0.0f));
        M5 = glm::rotate(M5, angleY_, glm::vec3(0.0f, 1.0f, 0.0f));
        M5 = glm::scale(M5, glm::vec3(0.20f));

        shader.setUniform("modelMatrix", M5);
        shader.use();
        models_.at("login").draw();
        ShaderProgram::unuse();
    }
}
