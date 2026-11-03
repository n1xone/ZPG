#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Window {
public:
    Window();
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool shouldClose() const;
    bool isKeyPressed(int key) const;
    bool isMouseButtonPressed(int button) const;
    void swapBuffers() const;
    void pollEvents() const;
    void framebufferSize(int& width, int& height) const;
    void cursorPosition(double& x, double& y) const;
    double time() const;

private:
    GLFWwindow* window_ = nullptr;
};
