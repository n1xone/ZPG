#include "Application.h"

void Application::run()
{
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilMask(0xFF);

    double lastTime = window_.time();

    double lastMouseX;
    double lastMouseY;
    window_.cursorPosition(lastMouseX, lastMouseY);

    bool wasLeftPressed = false;
    bool wasDeletePressed = false;
    while (!window_.shouldClose())
    {
        const double currentTime = window_.time();
        const float deltaTime = static_cast<float>(currentTime - lastTime);

        double mouseX;
        double mouseY;

        window_.cursorPosition(mouseX, mouseY);
        float mouseDeltaX = static_cast<float>(mouseX - lastMouseX);
        float mouseDeltaY = static_cast<float>(mouseY - lastMouseY);

        if (!window_.isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
        {
            mouseDeltaX = 0.0f;
            mouseDeltaY = 0.0f;
        }
        lastMouseX = mouseX;
        lastMouseY = mouseY;

        bool leftPressed = window_.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT);

        if (leftPressed && !wasLeftPressed)
        {
            requestPick();
        }

        wasLeftPressed = leftPressed;

        lastTime = currentTime;
        updateScene(deltaTime, static_cast<float>(currentTime), mouseDeltaX, mouseDeltaY);
        drawFrame();

        const bool deletePressed = window_.isKeyPressed(GLFW_KEY_DELETE);
        if (deletePressed && !wasDeletePressed)
            deleteSelectedObject();
        wasDeletePressed = deletePressed;

        window_.swapBuffers();
        window_.pollEvents();
    }
}

void Application::updateScene(float deltaTime, float time, float mouseDeltaX, float mouseDeltaY)
{
    const float step = 0.9f * deltaTime;
    float movementX = 0.0f;
    float movementY = 0.0f;
    float scaleChange = 0.0f;

    if (window_.isKeyPressed(GLFW_KEY_W))
        movementY += step;
    if (window_.isKeyPressed(GLFW_KEY_S))
        movementY -= step;
    if (window_.isKeyPressed(GLFW_KEY_A))
        movementX -= step;
    if (window_.isKeyPressed(GLFW_KEY_D))
        movementX += step;

    if (window_.isKeyPressed(GLFW_KEY_Q))
        scaleChange -= step;
    if (window_.isKeyPressed(GLFW_KEY_E))
        scaleChange += step;

    selectScene();

    scene_.update(movementX, movementY, scaleChange, time, mouseDeltaX, mouseDeltaY);
}

void Application::drawFrame()
{
    int width, height;
    window_.framebufferSize(width, height);
    if (width > 0 && height > 0)
    {
        glViewport(0, 0, width, height);

        glStencilMask(0xFF);
        glClearStencil(0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        scene_.draw(static_cast<float>(width) / height);

        if (pickRequested_)
        {
            pickObject();
            pickRequested_ = false;
        }
    }
}

void Application::selectScene()
{
    const struct
    {
        int key;
        int scene;
    } shortcuts[] = {
        {GLFW_KEY_1, 1},   {GLFW_KEY_2, 2},   {GLFW_KEY_3, 3},   {GLFW_KEY_4, 4}, {GLFW_KEY_5, 5},
        {GLFW_KEY_6, 6},   {GLFW_KEY_7, 7},   {GLFW_KEY_8, 8},   {GLFW_KEY_9, 9}, {GLFW_KEY_0, 0},
        {GLFW_KEY_F1, 10}, {GLFW_KEY_F2, 11}, {GLFW_KEY_F3, 12},
    };
    for (const auto& shortcut : shortcuts)
        if (window_.isKeyPressed(shortcut.key) && scene_.setScene(shortcut.scene))
            selectedID_ = 0;
}

void Application::deleteSelectedObject()
{
    if (selectedID_ == 0)
        return;

    if (scene_.removePlacedObject(selectedID_))
    {
        printf("Deleted object ID: %u\n", selectedID_);
        selectedID_ = 0;
    }
    else
    {
        printf("Only placed objects can be deleted.\n");
    }
}

void Application::requestPick()
{
    window_.cursorPosition(clickX_, clickY_);
    pickRequested_ = true;
}

void Application::pickObject()
{
    int windowW, windowH;
    int bufferW, bufferH;

    window_.windowSize(windowW, windowH);
    window_.framebufferSize(bufferW, bufferH);

    if (windowW <= 0 || windowH <= 0 || bufferW <= 0 || bufferH <= 0)
        return;

    int x = static_cast<int>(clickX_ * bufferW / windowW);
    int y = bufferH - 1 - static_cast<int>(clickY_ * bufferH / windowH);

    if (x < 0 || x >= bufferW || y < 0 || y >= bufferH)
        return;

    GLuint index = 0;
    GLfloat depth = 1.0f;

    glReadPixels(x, y, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);

    glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

    handlePick(index, depth, x, y, bufferW, bufferH);
}

void Application::handlePick(unsigned int index, float depth, int x, int y, int width, int height)
{
    const bool placingMoving = window_.isKeyPressed(GLFW_KEY_M);
    const bool placingTree = window_.isKeyPressed(GLFW_KEY_T);

    if (!placingMoving && !placingTree)
    {
        selectedID_ = index;
        printf("Selected object ID: %u, depth: %f\n", index, depth);
        glm::vec3 position;
        if (scene_.screenToWorld(x, y, depth, width, height, position))
            printf("unProject [%f, %f, %f]\n", position.x, position.y, position.z);
        return;
    }

    if (!scene_.supportsPlacement())
    {
        printf("Placement is available in the forest (3), classroom (F2), and terrain (F3).\n");
        return;
    }

    glm::vec3 position;
    if (!scene_.screenToWorld(x, y, depth, width, height, position))
        return;

    printf("unProject [%f, %f, %f]\n", position.x, position.y, position.z);
    if (placingMoving)
    {
        const auto result = scene_.setMovingPoint(position);
        if (result == Scene::MovingPointResult::StartSelected)
            printf("Point A selected. M + click another point to add a moving object.\n");
        else if (result == Scene::MovingPointResult::Created)
            printf("New moving object added!\n");
        else
            printf(
                "Cannot add: choose a different point or delete a placed object to free an ID.\n");
    }
    else if (placingTree)
    {
        printf(scene_.addTreeAt(position)
                   ? "Tree placed!\n"
                   : "Cannot place tree: delete a placed object to free an ID.\n");
    }
}
