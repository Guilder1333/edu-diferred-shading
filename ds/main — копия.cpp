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

#define DS_VERTEX_SHADER "#version 450\n" \
	"layout(location = 0) in vec3 position;\n" \
	"layout(location = 1) in vec2 vertTexCoord;\n" \
	"uniform mat4 mvp;" \
	"out vec2 fragTexCoord;" \
	"out gl_PerVertex { vec4 gl_Position; };" \
	"void main(void) {" \
	"	fragTexCoord = vertTexCoord;" \
	"	gl_Position = mvp * vec4(position, 1.0f);" \
	"}"

using namespace std;

ofstream logFile;

glm::mat4 camera(const glm::vec3 &translate, const glm::vec3 &rotate)
{
    glm::mat4 View = glm::translate(glm::mat4(), translate);
    View = glm::rotate(View, rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
    View = glm::rotate(View, rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
    View = glm::rotate(View, rotate.z, glm::vec3(0.0f, 0.0f, 1.0f));
	return View;
}

GLuint loadShader(string fileName, GLenum shaderType);
void printProgramLog(GLuint program);
void sdlClear(SDL_Window *window, SDL_GLContext context) {
    if (context != 0) {
        SDL_GL_DeleteContext(context);
    }
    if (window != nullptr) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

void print(glm::mat3 m) {
    float *v = glm::value_ptr(m);
    for(int i = 0, k = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++, k++) {
            logFile << v[k] << ' ';
        }
        logFile << endl;
    }
}

int main(int, char *[])
{
	logFile.open("log.log");

    logFile << "Hello World!" << endl;

    SDL_Window* window;
    SDL_GLContext context;
    if (SDL_Init(SDL_INIT_VIDEO)) {
        logFile << "Failed to initialize SDL2." << endl;
        return 1;
    }

	window = SDL_CreateWindow("Deferred Shading and Shadow Maps", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (window == 0) {
		logFile << "Failed to create window." << endl;
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
		logFile << SDL_GetError() << endl;
        sdlClear(window, 0);
		return 1;
	}

	GLenum error = glewInit();
	if (error != GL_NO_ERROR) {
		logFile << "Failed to initialize GLEW." << endl;
        sdlClear(window, context);
		return 1;
	}

	glClearColor(0.0f, 0.1f, 0.5f, 1.0f);

	GLuint vs = loadShader("ForwardVertexShader.glsl", GL_VERTEX_SHADER);
    GLuint fs = loadShader("ForwardFragmentShader.glsl", GL_FRAGMENT_SHADER);
    if (vs == 0 || fs == 0) {
        logFile << "Failed to create shaders" << endl;
        sdlClear(window, context);
        return 1;
    }

	GLuint program = glCreateProgram();
	glAttachShader(program, fs);
	glAttachShader(program, vs);
	glLinkProgram(program);
	GLint params;
	glGetProgramiv(program, GL_LINK_STATUS, &params);
	if (params != GL_TRUE) {
		logFile << "Failed to link program" << endl;
		printProgramLog(program);
		glDeleteProgram(program);
        sdlClear(window, context);
        return 1;
	}

    GLuint svs = loadShader("ShadowVertexShader.glsl", GL_VERTEX_SHADER);
    GLuint sfs = loadShader("ShadowFragmentShader.glsl", GL_FRAGMENT_SHADER);
    if (svs == 0 || sfs == 0) {
        logFile << "Failed to create shaders" << endl;
        sdlClear(window, context);
        return 1;
    }

    GLuint shadowProgram = glCreateProgram();
    glAttachShader(shadowProgram, sfs);
    glAttachShader(shadowProgram, svs);
    glLinkProgram(shadowProgram);
    glGetProgramiv(shadowProgram, GL_LINK_STATUS, &params);
    if (params != GL_TRUE) {
        logFile << "Failed to link program" << endl;
        printProgramLog(shadowProgram);
        glDeleteProgram(shadowProgram);
        sdlClear(window, context);
        return 1;
    }

	const GLint modelViewLoc = glGetUniformLocation(program, "modelView");
    const GLint modelLoc = glGetUniformLocation(program, "modelMatrix");
    const GLint normalLoc = glGetUniformLocation(program, "normalMatrix");
    const GLint biasMvpLoc = glGetUniformLocation(program, "shadowMvp");
	const GLint textureLoc = glGetUniformLocation(program, "tex");
    const GLint shadowTextureLoc = glGetUniformLocation(program, "shadowTex");
	const GLint colorLoc = glGetUniformLocation(program, "color");
	const GLint projLoc = glGetUniformLocation(program, "proj");
	const GLint lightLoc = glGetUniformLocation(program, "lightPos");
    const GLint stateLoc = glGetUniformLocation(program, "state");
    const GLint fragStateLoc = glGetUniformLocation(program, "fragState");

    const GLint shadowMvpLoc = glGetUniformLocation(shadowProgram, "mvp");

	Object object("Wood.3ds");
    Mesh sphere("sphere.3ds", "Sphere");
    Mesh plane("plane.3ds", "Plane");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

    GLuint shadowFramebuffer = 0;
    glGenFramebuffers(1, &shadowFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFramebuffer);
    GLuint depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        logFile << "Failed to create shadow framebuffer." << endl;
        sdlClear(window, context);
        return 1;
    }

    const float w = 20.0f;
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 1000.f);
    glm::mat4 shadowProjection = glm::ortho(-w, w, -w, w, 0.f, 100.f);
	glUseProgram(program);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(Projection));
    glm::mat4 biasMatrix(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0);

    object.setMatrix(glm::rotate(glm::mat4(), -3.14f/2, glm::vec3(1.0f, 0.0f, 0.0f)));

    bool terminated = false;
    SDL_Event event;
    float camZ = 0;
	float camX = 0;
    float camY = 0;
    float planeZ = 30.f;
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
            if (rendering == 3) {
                planeZ += speed;
            } else {
                camZ += speed;
            }
		}
		else if (scancodes[SDL_SCANCODE_S]) {
            if (rendering == 3) {
                planeZ -= speed;
            } else {
                camZ -= speed;
            }
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
            if (rendering > 3) {
                rendering = 0;
            }
        }

        if (rendering == 0 || rendering == 3) {
            glBindFramebuffer(GL_FRAMEBUFFER, shadowFramebuffer);
            glViewport(0, 0, 1024, 1024);
            glClear(GL_DEPTH_BUFFER_BIT);
        } else {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, 800, 600);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        // first pass: shadow
        glUseProgram(shadowProgram);

        //12.5f
        glm::vec3 lightPos = glm::vec3(-camX, camY, -camZ);
        glm::vec3& center = glm::vec3(0.f, 0.f, 0.f); //glm::vec3(model * glm::vec4(0.f, 0.f, 0.f, 1.f));
        glm::mat4& shadowViewMatrix = glm::lookAt(glm::vec3(lightPos), center, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4& shadowMVP = shadowProjection * shadowViewMatrix;
        //glUniformMatrix4fv(shadowMvpLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP));

        vector<Mesh*> &meshes = object.getMeshes();
        for (auto it = meshes.cbegin(); it != meshes.cend(); it++) {
            Mesh *m = (Mesh *)*it;
            /*glm::vec3 center = glm::vec3(m->getMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
            glm::mat4 shadowViewMatrix = glm::lookAt(glm::vec3(light), center, glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 shadowMVP = shadowProjection * shadowViewMatrix;*/
            glm::mat4& model = m->getMatrix();
            glUniformMatrix4fv(shadowMvpLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP * model));
            m->display();
        }

        glm::mat4 view = camera(glm::vec3(0, 10.f, -50.0f), glm::vec3(0, -3.14f/4, 0));
        if (rendering == 0 || rendering == 2) {
            glViewport(0, 0, 800, 600);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUseProgram(program);

            glUniform1i(fragStateLoc, 0);
            glUniform1i(textureLoc, 0);
            glUniform1i(shadowTextureLoc, 1);
            glUniform3fv(lightLoc, 1, glm::value_ptr(lightPos));
            glUniform1f(stateLoc, rendering * 1.f);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, depthTexture);

            for (auto it = meshes.cbegin(); it != meshes.cend(); it++) {
                Mesh *m = (Mesh *)*it;
                glm::mat4& model = m->getMatrix();
                glm::mat4& modelView = view * model;
                glm::mat3& normal = glm::mat3(glm::transpose(glm::inverse(model)));
                glUniformMatrix4fv(modelViewLoc, 1, GL_FALSE, glm::value_ptr(modelView));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniformMatrix4fv(biasMvpLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP * model));
                glUniformMatrix3fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
                glUniform4fv(colorLoc, 1, glm::value_ptr(m->diffuseColor()));

                m->display();
            }

            if (rendering == 0) {
                glm::mat4& model = glm::translate(glm::mat4(), glm::vec3(lightPos));
                glm::mat4& modelView = view * model;
                glm::mat3& normal = glm::mat3(glm::transpose(glm::inverse(model)));
                glUniformMatrix4fv(modelViewLoc, 1, GL_FALSE, glm::value_ptr(modelView));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniformMatrix4fv(biasMvpLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4()));
                glUniformMatrix3fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
                glUniform4fv(colorLoc, 1, glm::value_ptr(sphere.diffuseColor()));
                sphere.display();
            }
        }

        if (rendering == 3) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, 800, 600);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUseProgram(program);

            glUniform1i(fragStateLoc, 1);
            glUniform1i(textureLoc, 0);
            glUniform1i(shadowTextureLoc, 1);
            glUniform3fv(lightLoc, 1, glm::value_ptr(lightPos));
            glUniform1f(stateLoc, rendering * 1.f);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, depthTexture);

            glm::mat4& model = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, planeZ));
            glm::mat4& modelView = view * model;
            glm::mat3& normal = glm::mat3(glm::transpose(glm::inverse(model)));
            glUniformMatrix4fv(modelViewLoc, 1, GL_FALSE, glm::value_ptr(modelView));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(biasMvpLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4()));
            glUniformMatrix3fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
            glUniform4fv(colorLoc, 1, glm::value_ptr(plane.diffuseColor()));
            plane.display();
            glUniform1i(fragStateLoc, 0);
        }

		SDL_GL_SwapWindow(window);
    }

    sdlClear(window, context);
    return 0;
}

void printProgramLog(GLuint program) {
	const int maxLength = 2048;
	int length = 0;
	char log[maxLength];
	glGetProgramInfoLog(program, maxLength, &length, log);
	logFile << "program info log for GL index" << program << ":" << endl << log << endl;
}

GLuint loadShader(string fileName, GLenum shaderType) {
	GLint params;
	ifstream f;
    f.open(fileName, ifstream::binary | ifstream::ate);
	if (!f.is_open()) {
        logFile << "Shader file '" << fileName << "'' not found." << endl;
		return 0;
	}
    streamoff length = f.tellg();
	f.seekg(ifstream::beg);
    char *str = new char[length + 1];
	f.read(str, length);
    str[length] = 0;
	f.close();

	// somehow it has better error check
    GLuint program = glCreateShaderProgramv(shaderType, 1, &str);
	glGetProgramiv(program, GL_LINK_STATUS, &params);
	if (params != GL_TRUE) {
		logFile << "Failed to link vertex shader program" << endl;
		printProgramLog(program);
		glDeleteProgram(program);
		return 0;
	}
    glDeleteProgram(program);

	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &str, NULL);
	glCompileShader(shader);
	delete[]str;
	return shader;
}
