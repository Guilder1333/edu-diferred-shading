#define GLEW_NO_GLU

#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <string>
#include <SDL2/SDL.h>
#include <vec3.hpp> // glm::vec3
#include <vec4.hpp> // glm::vec4
#include <mat4x4.hpp> // glm::mat4
#include <gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <gtc/type_ptr.hpp>
#include "Cube.h"
#include "Triangle.h"
#include "Mesh.h"
#include "Object.h"
#include "RenderPlane.h"
#include "Logger.h"
#include "RenderScene.h"

void sdlClear(SDL_Window *window, SDL_GLContext context) {
    Logger::destroy();
    if (context != 0) {
        SDL_GL_DeleteContext(context);
    }
    if (window != nullptr) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

int main(int, char *[])
{
    Logger::initialize(std::string("log.log"));

    Logger::log(std::string("Hello World!"));

    SDL_Window* window;
    SDL_GLContext context;
    if (SDL_Init(SDL_INIT_VIDEO)) {
        Logger::log(std::string("Failed to initialize SDL2."));
        return 1;
    }

    const GLint windowWidth = 800;
    const GLint windowHeight = 600;

    window = SDL_CreateWindow("Deferred Shading and Shadow Maps", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (window == 0) {
        Logger::log("Failed to create window.");
		SDL_Quit();
		return 1;
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

	context = SDL_GL_CreateContext(window);
	if (context == 0) {
        Logger::log(std::string(SDL_GetError()));
        sdlClear(window, 0);
		return 1;
	}

	GLenum error = glewInit();
	if (error != GL_NO_ERROR) {
        Logger::log("Failed to initialize GLEW.");
        sdlClear(window, context);
		return 1;
	}

    RenderScene *scene = new RenderScene(windowWidth, windowHeight);
    if (!scene->initialize()) {
        Logger::log("Failed to initialize render scene.");
        delete scene;
        sdlClear(window, context);
        return 1;
    }
    ResourceManager *manager = new ResourceManager();
    ResourceManager::setInstnace(manager);
    Object *object = manager->load3dsObject(std::string("Wood.3ds"));
    object->setRotation(-3.14f/2, .0f, .0f);
    Mesh *sphere = manager->load3dsMesh(std::string("sphere.3ds"), std::string("Sphere"));
    Mesh *plane = manager->load3dsMesh(std::string("plane.3ds"), std::string("Plane"));
    Light *light = manager->createLight();

    Camera* camera = new Camera();
    camera->setPerspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 1000.f);
    camera->setPosition(0.f, 0.f, -50.0f);
    camera->setRotation(0.f, -3.14f/4, 0.f);

    scene->setCamera(camera);
    scene->setLight(light);

    scene->addRenderObject(object);
    scene->addRenderObject(sphere);

    bool terminated = false;
    SDL_Event event;
    float camZ = 0;
	float camX = 0;
    float camY = 5.f;
    const float planeSpeed = 0.05f;
    int rendering = 0;
    const float speed = 0.05f;
    bool scancodes[SDL_NUM_SCANCODES] = {0};
    while (!terminated) {
		while (SDL_PollEvent(&event)) {
			if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
				terminated = true;
			}
			if (event.type == SDL_KEYDOWN) {
				scancodes[event.key.keysym.scancode] = true;
			}
			else if (event.type == SDL_KEYUP) {
				scancodes[event.key.keysym.scancode] = false;
			}
		}

		if (scancodes[SDL_SCANCODE_W]) {
            camZ += speed;
		}
		else if (scancodes[SDL_SCANCODE_S]) {
            camZ -= speed;
		}
		if (scancodes[SDL_SCANCODE_D]) {
            camX -= speed;
		}
		else if (scancodes[SDL_SCANCODE_A]) {
            camX += speed;
		}
        if (scancodes[SDL_SCANCODE_R]) {
            camY += speed;
        } else if (scancodes[SDL_SCANCODE_F]) {
            camY -= speed;
        }

        if (scancodes[SDL_SCANCODE_Q]) {
            rendering++;
            scancodes[SDL_SCANCODE_Q] = false;
            if (rendering > 1) {
                rendering = 0;
            }
        }

        light->setPosition(-camX, camY, -camZ);

        scene->renderScene();

		SDL_GL_SwapWindow(window);
    }

    delete manager;
    delete scene;
    sdlClear(window, context);
    return 0;
}
