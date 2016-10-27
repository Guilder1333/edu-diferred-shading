#define GLEW_NO_GLU
#include "Window.h"

int main(int, char *[])
{
    const GLint windowWidth = 800;
    const GLint windowHeight = 600;
    Window *window = new Window(windowWidth, windowHeight, std::string("Deferred Shading and Shadow Maps"));

    window->run();

    const int result = window->getResult();
    delete window;
    return result;
}
