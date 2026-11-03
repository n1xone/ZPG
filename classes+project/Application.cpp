#include "Application.h"

void Application::run()
{
    glEnable(GL_DEPTH_TEST);
    
    
    double lastTime = window_.time();

    double lastMouseX;
    double lastMouseY;
    window_.cursorPosition(lastMouseX, lastMouseY);

    while (!window_.shouldClose()) {
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

        lastTime = currentTime;
        const float step = 0.9f * deltaTime; //movement speed
        float movementX = 0.0f;
        float movementY = 0.0f;
        float scaleChange = 0.0f; //count of difference
        
        if (window_.isKeyPressed(GLFW_KEY_W)) movementY += step;
        if (window_.isKeyPressed(GLFW_KEY_S)) movementY -= step;
        if (window_.isKeyPressed(GLFW_KEY_A)) movementX -= step;
        if (window_.isKeyPressed(GLFW_KEY_D)) movementX += step;

        if (window_.isKeyPressed(GLFW_KEY_Q)) scaleChange -= step;
        if (window_.isKeyPressed(GLFW_KEY_E)) scaleChange += step;

        if (window_.isKeyPressed(GLFW_KEY_1)) scene_.setScene(1);
        if (window_.isKeyPressed(GLFW_KEY_2)) scene_.setScene(2);
        if (window_.isKeyPressed(GLFW_KEY_3)) scene_.setScene(3);
        if (window_.isKeyPressed(GLFW_KEY_4)) scene_.setScene(4);
        if (window_.isKeyPressed(GLFW_KEY_5)) scene_.setScene(5);
        if (window_.isKeyPressed(GLFW_KEY_6)) scene_.setScene(6);
        if (window_.isKeyPressed(GLFW_KEY_7)) scene_.setScene(7);

        scene_.update(movementX, movementY, scaleChange, static_cast<float>(window_.time()), mouseDeltaX, mouseDeltaY);


        int width, height;
        window_.framebufferSize(width, height);
        if (width > 0 && height > 0) {
            glViewport(0, 0, width, height);
            scene_.draw(static_cast<float>(width) / height);
        }
        window_.swapBuffers();
        window_.pollEvents();
    }
}
