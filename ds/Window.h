#pragma once

#include <SDL2/SDL.h>
#include <string>
#include "RenderScene.h"

class Window
{
public:
    Window(const int width, const int height, const std::string &name);
    ~Window();
    void run();
    int getResult() const;
protected:
    bool initialize();
    void destroy();
    void update(const float time);
    void render(const float time);
private:
    SDL_Window* window;
    SDL_GLContext context;
    std::string name;
    int result;
    int width;
    int height;
    bool terminated;
    bool scancodes[SDL_NUM_SCANCODES] = {0};

    // application specific
    float camX;
    float camY;
    float camZ;
    RenderScene *scene;
    ResourceManager *manager;
    Camera* camera;
    Object *object;
    Mesh *sphere;
    Mesh *plane;
    Light *light;
};
