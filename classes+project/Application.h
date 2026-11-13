#pragma once
#include "Window.h"
#include "Scene.h"

class Application
{
public:
    void run();

private:
    void updateScene(float deltaTime, float time, float mouseDeltaX, float mouseDeltaY);
    void drawFrame();
    void selectScene();
    void requestPick();
    void pickObject();
    void deleteSelectedObject();
    void handlePick(unsigned int id, float depth, int x, int y, int width, int height);

    // Members are destroyed in reverse order: the GL context must outlive the scene.
    Window window_;
    Scene scene_;

    bool pickRequested_ = false;
    double clickX_ = 0.0;
    double clickY_ = 0.0;

    unsigned int selectedID_ = 0;
};
