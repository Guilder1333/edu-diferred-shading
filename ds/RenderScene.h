#pragma once

#include <string>
#include <vector>
#include "ResourceManager.h"
#include "Camera.h"
#include "Texture.h"
#include "Material.h"
#include "Renderable.h"
#include "Framebuffer.h"

#include <GL/glew.h>

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
    const Renderable *renderPlane;
    glm::mat4 renderPlaneMatrix;

    const Texture *depthTexture;
    const Texture *colorTexture;
    const Texture *positionTexture;
    const Texture *normalTexture;
    const Framebuffer *shadowFramebuffer;
    const Framebuffer *firstPassBuffer;
    const GLint windowWidth;
    const GLint windowHeight;
    const GLint textureWidth;
    const GLint textureHeight;
};
