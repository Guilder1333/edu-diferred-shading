#pragma once

#include <string>
#include <vector>
#include "ResourceManager.h"
#include "Camera.h"
#include "Texture.h"
#include "Material.h"
#include "Renderable.h"
#include "gl/glew.h"

class RenderScene
{
public:
    RenderScene(const GLint windowWidth, const GLint windowHeight);
    virtual ~RenderScene();

    bool initialize();

    void addRenderObject(const Renderable *);
    void addRenderObject(const Object *);
    void setCamera(const Camera *);
    void setLight(const Light *light);
    void renderScene();
private:
    std::vector<const Renderable *> renderables;
    class FirstPassProgram *firstPass;
    class SecondPassProgram *secondPass;
    class ShadowPassProgram *shadowPass;
    const Camera *camera;
    const Light *light;
    Renderable *renderPlane;
    glm::mat4 renderPlaneMatrix;

    Texture *depthTexture;
    Texture *colorTexture;
    Texture *positionTexture;
    Texture *normalTexture;
    GLuint shadowFramebuffer;
    GLuint firstPassBuffer;
    GLint windowWidth;
    GLint windowHeight;
    GLint textureWidth;
    GLint textureHeight;
};
