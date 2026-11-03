#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include "Window.h"

#include <cstdio>
#include <stdexcept>

Window::Window()
{
    if (!glfwInit())
        throw std::runtime_error("GLFW initialization failed");

    window_ = glfwCreateWindow(1200, 900, "ZPG", nullptr, nullptr);
    if (!window_) {
        glfwTerminate();
        throw std::runtime_error("Unable to create window");
    }

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        glfwDestroyWindow(window_);
        glfwTerminate();
        throw std::runtime_error("GLAD initialization failed");
    }
        
    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);
    glViewport(0, 0, width, height);

    std::printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    std::printf("Vendor: %s\n", glGetString(GL_VENDOR));
    std::printf("Renderer: %s\n", glGetString(GL_RENDERER));
    std::printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    std::printf("Using GLFW %i.%i.%i\n", major, minor, revision);
}

Window::~Window()
{
    glfwDestroyWindow(window_);
    glfwTerminate();
}

bool Window::shouldClose() const { return glfwWindowShouldClose(window_); }
bool Window::isKeyPressed(int key) const { return glfwGetKey(window_, key) == GLFW_PRESS; }
bool Window::isMouseButtonPressed(int button) const { return glfwGetMouseButton(window_, button) == GLFW_PRESS; }
void Window::swapBuffers() const { glfwSwapBuffers(window_); }
void Window::pollEvents() const { glfwPollEvents(); }
double Window::time() const { return glfwGetTime(); }


void Window::framebufferSize(int& width, int& height) const
{
    glfwGetFramebufferSize(window_, &width, &height);
    //glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //dont locking my cursor

    if (width > 0 && height > 0)
        glViewport(0, 0, width, height);
}

void Window::cursorPosition(double& x, double& y) const
{
    glfwGetCursorPos(window_, &x, &y);
}
