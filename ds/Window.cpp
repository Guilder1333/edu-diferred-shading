#include "Window.h"
#include "Logger.h"
#include <gl/glew.h>
#include <chrono>

Window::Window(const int width, const int height, const std::string &name)
    : window(nullptr), width(width), height(height), name(name), result(1),
      scene(nullptr), terminated(false), camera(nullptr), manager(nullptr),
      light(nullptr)
{

}

Window::~Window()
{
    Logger::destroy();

    this->destroy();

    if (this->context != 0) {
        SDL_GL_DeleteContext(context);
    }
    if (this->window != nullptr) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

void Window::run()
{
    Logger::initialize(std::string("log.log"));

    if (SDL_Init(SDL_INIT_VIDEO)) {
        Logger::log(std::string("Failed to initialize SDL2."));
        this->result = 1;
        return;
    }

    this->window = SDL_CreateWindow(this->name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->width, this->height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    if (this->window == nullptr) {
        Logger::log("Failed to create window.");
        this->result = 2;
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);

    SDL_GL_SetSwapInterval(1);

    this->context = SDL_GL_CreateContext(this->window);
    if (this->context == 0) {
        Logger::log(std::string(SDL_GetError()));
        this->result = 3;
        return;
    }

    GLenum error = glewInit();
    if (error != GL_NO_ERROR) {
        Logger::log("Failed to initialize GLEW.");
        this->result = 4;
        return;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEBUG_OUTPUT);
    //glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    if (!this->initialize()) {
        Logger::log("Failed to initialize scene.");
        this->result = 5;
        return;
    }

    this->result = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> time = std::chrono::high_resolution_clock::now();
    SDL_Event event;
    while (!this->terminated) {
        while (SDL_PollEvent(&event)) {
            if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                this->terminated = true;
            }
            if (event.type == SDL_KEYDOWN) {
                this->scancodes[event.key.keysym.scancode] = true;
            }
            else if (event.type == SDL_KEYUP) {
                this->scancodes[event.key.keysym.scancode] = false;
            }
        }

        const float d = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - time).count() / 1000000000.0f;
        time = std::chrono::high_resolution_clock::now();

        this->update(d);
        this->render(d);

        SDL_GL_SwapWindow(this->window);
    }
}

bool Window::initialize()
{
    this->scene = new RenderScene(this->width, this->height);
    if (!this->scene->initialize()) {
        Logger::log("Failed to initialize render scene.");
        return false;
    }
    this->manager = new ResourceManager();
    ResourceManager::setInstnace(this->manager);
    this->object = this->manager->load3dsObject(std::string("Wood.3ds"));
    this->object->setRotation(glm::radians(-90.f), .0f, .0f);
    this->sphere = this->manager->load3dsMesh(std::string("sphere.3ds"), std::string("Sphere"));
    this->plane = this->manager->load3dsMesh(std::string("plane.3ds"), std::string("Plane"));
    this->light = this->manager->createLight();

    this->camera = new Camera();
    this->camera->setPerspective(glm::radians(45.0f), 4.0f / 3.0f, 1.f, 1000.f);
    this->camera->setRotation(glm::radians(30.f), glm::radians(-45.f), 0.f);

    this->scene->setCamera(this->camera);
    this->scene->setLight(this->light);

    this->scene->addRenderObject(this->object);
    this->scene->addRenderObject(this->sphere);

    this->camX = 0;
    this->camY = 0;
    this->camZ = -50.0f;
    this->lightX = 0;
    this->lightY = 5.f;
    this->lightZ = 0;
    return true;
}

void Window::destroy()
{
    if (this->scene != nullptr) {
        delete this->scene;
    }
    if (this->manager != nullptr) {
        delete this->manager;
    }
    if (this->camera != nullptr) {
        delete this->camera;
    }
}

void Window::update(const float )
{
    const float speed = 0.05f;
    if (this->scancodes[SDL_SCANCODE_W]) {
        this->camZ += speed;
    }
    else if (this->scancodes[SDL_SCANCODE_S]) {
        this->camZ -= speed;
    }
    if (this->scancodes[SDL_SCANCODE_D]) {
        this->camX -= speed;
    }
    else if (this->scancodes[SDL_SCANCODE_A]) {
        this->camX += speed;
    }
    if (this->scancodes[SDL_SCANCODE_R]) {
        this->camY += speed;
    } else if (this->scancodes[SDL_SCANCODE_F]) {
        this->camY -= speed;
    }

    if (this->scancodes[SDL_SCANCODE_UP]) {
        this->lightZ -= speed;
    }
    else if (this->scancodes[SDL_SCANCODE_DOWN]) {
        this->lightZ += speed;
    }
    if (this->scancodes[SDL_SCANCODE_RIGHT]) {
        this->lightX += speed;
    }
    else if (this->scancodes[SDL_SCANCODE_LEFT]) {
        this->lightX -= speed;
    }
    if (this->scancodes[SDL_SCANCODE_PAGEUP]) {
        this->lightY += speed;
    } else if (this->scancodes[SDL_SCANCODE_PAGEDOWN]) {
        this->lightY -= speed;
    }

    this->light->setPosition(this->lightX, this->lightY, this->lightZ);
    this->sphere->setPosition(this->lightX, this->lightY, this->lightZ);
    this->camera->setPosition(this->camX, this->camY, this->camZ);
}

void Window::render(const float )
{
    this->scene->renderScene();
}

int Window::getResult() const
{
    return this->result;
}
