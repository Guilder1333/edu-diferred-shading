#include "RenderScene.h"

#include "FragmentShader.h"
#include "VertexShader.h"
#include "ShaderProgram.h"
#include "ProgramVariable.h"
#include "Logger.h"
#include "RenderPlane.h"
#include "gtc/matrix_transform.hpp"

#define SHADOW_TEXTURE_WIDTH 1024
#define SHADOW_TEXTURE_HEIGHT 1024

const glm::mat4 biasMatrix(
    0.5f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.5f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.5f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f);

class FirstPassProgram : public ShaderProgram {
public:
    ProgramVariable<glm::mat4> *projection;
    ProgramVariable<glm::mat4> *modelView;
    ProgramVariable<glm::mat4> *modelMatrix;
    ProgramVariable<glm::mat3> *normalMatrix;
    ProgramVariable<glm::mat4> *shadowMatrix;
    ProgramVariable<glm::vec4> *diffuseColor;
    ProgramVariable<Texture, 0> *texture;
    ProgramVariable<Texture, 1> *shadowTexture;
    FirstPassProgram()
        : ShaderProgram(), projection(nullptr), modelView(nullptr),
          modelMatrix(nullptr), normalMatrix(nullptr), shadowMatrix(nullptr),
          texture(nullptr), diffuseColor(nullptr)
    {

    }

    virtual ~FirstPassProgram()
    {
        if (this->projection != nullptr) {
            delete this->projection;
        }
        if (this->modelView != nullptr) {
            delete this->modelView;
        }
        if (this->modelMatrix != nullptr) {
            delete this->modelMatrix;
        }
        if (this->shadowMatrix != nullptr) {
            delete this->shadowMatrix;
        }
        if (this->normalMatrix != nullptr) {
            delete this->normalMatrix;
        }
        if (this->texture != nullptr) {
            delete this->texture;
        }
        if (this->shadowTexture != nullptr) {
            delete this->shadowTexture;
        }
        if (this->diffuseColor != nullptr) {
            delete this->diffuseColor;
        }
    }

    virtual bool isOk() const {
        if (ShaderProgram::isOk()) {
            return true; /*this->projection->isOk() && this->modelView->isOk()
                    && this->modelMatrix->isOk() && this->shadowMatrix->isOk()
                    && this->normalMatrix->isOk() && this->texture->isOk()
                    && this->shadowTexture->isOk() && this->diffuseColor->isOk();//*/
        }
        return false;
    }

protected:
    virtual void link()
    {
        ShaderProgram::link();
        if (ShaderProgram::isOk())
        {
            this->use();
            this->projection = new ProgramVariable<glm::mat4>(this, "proj");
            this->modelView = new ProgramVariable<glm::mat4>(this, "modelView");
            this->modelMatrix = new ProgramVariable<glm::mat4>(this, "modelMatrix");
            this->shadowMatrix = new ProgramVariable<glm::mat4>(this, "shadowMvp");
            this->normalMatrix = new ProgramVariable<glm::mat3>(this, "normalMatrix");
            this->diffuseColor = new ProgramVariable<glm::vec4>(this, "color");
            this->texture = new ProgramVariable<Texture, 0>(this, "tex");
            this->shadowTexture = new ProgramVariable<Texture, 1>(this, "shadowTex");
        }
    }
};

class SecondPassProgram : public ShaderProgram {
public:
    ProgramVariable<glm::mat4> *modelViewProjection;
    ProgramVariable<glm::mat4> *projection;
    ProgramVariable<glm::vec3> *lightPosition;
    ProgramVariable<Texture, 0> *positionTexture;
    ProgramVariable<Texture, 1> *normalTexture;
    ProgramVariable<Texture, 2> *colorTexture;
    SecondPassProgram()
        : ShaderProgram(), modelViewProjection(nullptr), positionTexture(nullptr),
          normalTexture(nullptr), colorTexture(nullptr), lightPosition(nullptr), projection(nullptr)
    {

    }

    virtual ~SecondPassProgram()
    {
        if (this->modelViewProjection != nullptr) {
            delete this->modelViewProjection;
        }
        if (this->positionTexture != nullptr) {
            delete this->positionTexture;
        }
        if (this->normalTexture != nullptr) {
            delete this->normalTexture;
        }
        if (this->colorTexture != nullptr) {
            delete this->colorTexture;
        }
        if (this->lightPosition != nullptr) {
            delete this->lightPosition;
        }
    }

    virtual bool isOk() const {
        if (ShaderProgram::isOk()) {
            return true; /*this->modelViewProjection->isOk() && this->positionTexture->isOk()
                    && this->normalTexture->isOk() && this->colorTexture->isOk()
                    && this->lightPosition->isOk();//*/
        }
        return false;
    }

protected:
    virtual void link()
    {
        ShaderProgram::link();
        if (ShaderProgram::isOk())
        {
            this->use();
            this->modelViewProjection = new ProgramVariable<glm::mat4>(this, "mvp");
            this->lightPosition = new ProgramVariable<glm::vec3>(this, "lightPos");
            this->positionTexture = new ProgramVariable<Texture, 0>(this, "posTex");
            this->normalTexture = new ProgramVariable<Texture, 1>(this, "normalTex");
            this->colorTexture = new ProgramVariable<Texture, 2>(this, "colorTex");
            this->projection = new ProgramVariable<glm::mat4>(this, "projection");
        }
    }
};

class ShadowPassProgram : public ShaderProgram
{
public:
    ProgramVariable<glm::mat4> *shadowMatrix;
    ShadowPassProgram()
        : ShaderProgram(), shadowMatrix(nullptr)
    {

    }

    virtual ~ShadowPassProgram()
    {
        if (this->shadowMatrix != nullptr) {
            delete this->shadowMatrix;
        }
    }

    virtual bool isOk() const {
        return ShaderProgram::isOk() && this->shadowMatrix->isOk();
    }

protected:
    virtual void link()
    {
        ShaderProgram::link();
        if (ShaderProgram::isOk())
        {
            this->use();
            this->shadowMatrix = new ProgramVariable<glm::mat4>(this, "mvp");
        }
    }
};

RenderScene::RenderScene(const int windowWidth, const int windowHeight)
    : firstPass(nullptr), secondPass(nullptr), shadowPass(nullptr), depthTexture(nullptr),
      windowWidth(windowWidth), windowHeight(windowHeight),
      renderPlane(nullptr), light(nullptr), colorTexture(nullptr),
      normalTexture(nullptr), positionTexture(nullptr),
      firstPassBuffer(nullptr), shadowFramebuffer(nullptr)
{
    GLint textureWidth = 2;
    GLint textureHeight = 2;
    while (textureWidth < windowWidth) {
        textureWidth *= 2;
    }

    while (textureHeight < windowHeight) {
        textureHeight *= 2;
    }
    this->textureWidth = textureWidth;
    this->textureHeight = textureHeight;
}

RenderScene::~RenderScene()
{
    if (this->firstPass != nullptr) {
        delete this->firstPass;
    }
    if (this->secondPass != nullptr) {
        delete this->secondPass;
    }
    if (this->shadowPass != nullptr) {
        delete this->shadowPass;
    }
    if (this->renderPlane != nullptr) {
        delete this->renderPlane;
    }
    if (this->depthTexture != nullptr) {
        delete this->depthTexture;
    }
    if (this->colorTexture != nullptr) {
        delete this->colorTexture;
    }
    if (this->positionTexture != nullptr) {
        delete this->positionTexture;
    }
    if (this->normalTexture != nullptr) {
        delete this->normalTexture;
    }
    if (this->firstPassBuffer != nullptr) {
        delete this->firstPassBuffer;
    }
    if (this->shadowFramebuffer != nullptr) {
        delete this->shadowFramebuffer;
    }
}

bool RenderScene::initialize()
{
    this->firstPass = new FirstPassProgram();
    this->firstPass->assign(new VertexShader(std::string("DSFPVertexShader.glsl")));
    this->firstPass->assign(new FragmentShader(std::string("DSFPFragmentShader.glsl")));
    if (!this->firstPass->isOk()) {
        return false;
    }

    this->secondPass = new SecondPassProgram();
    this->secondPass->assign(new VertexShader(std::string("DSSPVertexShader.glsl")));
    this->secondPass->assign(new FragmentShader(std::string("DSSPFragmentShader.glsl")));
    if (!this->secondPass->isOk()) {
        return false;
    }

    this->shadowPass = new ShadowPassProgram();
    this->shadowPass->assign(new VertexShader(std::string("ShadowVertexShader.glsl")));
    this->shadowPass->assign(new FragmentShader(std::string("ShadowFragmentShader.glsl")));
    if (!this->shadowPass->isOk()) {
        return false;
    }

    ShaderProgram::useProgram(nullptr);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Shadow pass
    this->shadowFramebuffer = new Framebuffer(SHADOW_TEXTURE_WIDTH, SHADOW_TEXTURE_HEIGHT);

    this->depthTexture = this->shadowFramebuffer->addBuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
    if (!this->shadowFramebuffer->initialize()) {
        Logger::log(std::string("Failed to create shadow framebuffer."));
        return false;
    }

    // First pass
    this->firstPassBuffer = new Framebuffer(this->textureWidth, this->textureHeight);

    this->positionTexture = this->firstPassBuffer->addBuffer(GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0);
    this->normalTexture = this->firstPassBuffer->addBuffer(GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT1);
    this->colorTexture = this->firstPassBuffer->addBuffer(GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT2);

    if(!this->firstPassBuffer->initialize()) {
        Logger::log(std::string("Failed to create first pass framebuffer."));
        return false;
    }

    Framebuffer::bindMain();

    this->renderPlane = new RenderPlane();

    const float planeX = (this->textureWidth - this->windowWidth)/2.f;
    const float planeY = (this->textureHeight - this->windowHeight)/2.f;
    const glm::mat4& planeProjection = glm::ortho(-this->windowWidth/2.f, this->windowWidth/2.f, -this->windowHeight/2.f, this->windowHeight/2.f, -1.f, 1.f);
    this->renderPlaneMatrix = planeProjection *
                glm::scale(glm::translate(glm::mat4(), glm::vec3(planeX, planeY, 1.f)), glm::vec3(this->textureWidth/2.f, this->textureHeight/2.f, -1.f));

    return true;
}

void RenderScene::addRenderObject(const Renderable *renderable)
{
    this->renderables.push_back(renderable);
}

void RenderScene::addRenderObject(const Object *renderable)
{
    const std::vector<const Mesh*> *meshes = renderable->getMeshes();
    for (auto it = meshes->cbegin(); it != meshes->cend(); it++) {
        this->renderables.push_back((const Mesh *)*it);
    }
}

void RenderScene::setCamera(const Camera *camera)
{
    this->camera = camera;
}

void RenderScene::setLight(const Light *light)
{
    this->light = light;
}

void RenderScene::renderScene()
{
    this->shadowFramebuffer->bind();
    glViewport(0, 0, SHADOW_TEXTURE_WIDTH, SHADOW_TEXTURE_HEIGHT);
    glClear(GL_DEPTH_BUFFER_BIT);
    this->shadowPass->use();

    const float w = 10.0f;
    const glm::mat4& shadowViewMatrix = glm::ortho(-w, w, -w, w, 0.f, 1000.f)
            * glm::lookAt(this->light->getPosition(), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0f, 1.0f, 0.0f));

    for (auto it = this->renderables.cbegin(); it != this->renderables.cend(); it++) {
        const Renderable *m = (Renderable *)*it;
        this->shadowPass->shadowMatrix->setValue(shadowViewMatrix * m->getMatrix());
        m->display();
    }

    this->firstPassBuffer->bind();
    glViewport(0, 0, this->textureWidth, this->textureHeight);
    glClearColor(1.0f, 0.1f, 0.5f, 1.0f);
    glViewport(0, 0, this->windowWidth, this->windowHeight);
    glClearColor(0.0f, 0.1f, 0.5f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::mat4& view = this->camera->getMatrix();
    this->firstPass->use();
    this->firstPass->shadowTexture->setValue(this->depthTexture);
    this->firstPass->projection->setValue(this->camera->getProjection());

    for (auto it = this->renderables.cbegin(); it != this->renderables.cend(); it++) {
        const Renderable *m = (Renderable *)*it;
        const glm::mat4& model = m->getMatrix();
        const glm::mat4& modelView = view * model;
        this->firstPass->modelView->setValue(modelView);
        this->firstPass->modelMatrix->setValue(model);
        this->firstPass->shadowMatrix->setValue(biasMatrix * shadowViewMatrix * model);
        this->firstPass->normalMatrix->setValue(glm::mat3(glm::transpose(glm::inverse(model))));
        if (m->getMaterial() == nullptr) {
            this->firstPass->texture->setValue(nullptr);
            this->firstPass->diffuseColor->setValue(glm::vec4(1.f, 1.f, 1.f, 1.f));
        } else {
            this->firstPass->texture->setValue(m->getMaterial()->getTexture());
            this->firstPass->diffuseColor->setValue(m->getMaterial()->getDiffuseColor());
        }

        m->display();
    }

    Framebuffer::bindMain();
    glViewport(0, 0, this->windowWidth, this->windowHeight);

    glClearColor(0.0f, 0.1f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->secondPass->use();
    this->secondPass->projection->setValue(glm::inverse(this->camera->getProjection() * this->camera->getMatrix()));
    this->secondPass->positionTexture->setValue(this->positionTexture);
    this->secondPass->normalTexture->setValue(this->normalTexture);
    this->secondPass->colorTexture->setValue(this->colorTexture);
    this->secondPass->lightPosition->setValue(this->light->getPosition());
    this->secondPass->modelViewProjection->setValue(this->renderPlaneMatrix);
    this->renderPlane->display();
}
