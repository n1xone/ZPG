#pragma once
#include "Window.h"
#include "Scene.h"

class Application {
public:
    void run();

private:
    // Members are destroyed in reverse order: the GL context must outlive the scene.
    Window window_;
    Scene scene_;
};
