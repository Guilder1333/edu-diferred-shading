#pragma once

#include <SDL2/SDL.h>

#include <string>

#include "RenderScene.h"

#define UPDATE_PER_SECOND 60
#define FRAME_PER_SECOND 30

class Window
{
public:
    Window(const GLint width, const GLint height, const std::string &name);
    ~Window();
    void run();
    int getResult() const;
protected:
    bool initialize();
    void destroy();
    void update(const float time);
    void render();
private:
    SDL_Window* window;
    SDL_GLContext context;
    std::string name;
    GLint result;
    GLint width;
    GLint height;
    GLboolean terminated;
    const Uint8 *scancodes;
    GLint mouseX;
    GLint mouseY;
    GLint mouseDeltaX;
    GLint mouseDeltaY;
    GLboolean mouseDown;

    // application specific
    GLfloat camX;
    GLfloat camY;
    GLfloat camZ;
    GLfloat camYaw;
    GLfloat camPitch;
    GLfloat lightX;
    GLfloat lightY;
    GLfloat lightZ;
    GLint lastMouseX;
    GLint lastMouseY;
    RenderScene *scene;
    ResourceManager *manager;
    Camera* camera;
    Object *object;
    Mesh *sphere;
    Object *plane;
    Light *light;
};
