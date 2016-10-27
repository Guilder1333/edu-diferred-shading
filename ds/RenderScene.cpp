#include "RenderScene.h"

#include "FragmentShader.h"
#include "VertexShader.h"
#include "ShaderProgram.h"
#include "ProgramVariable.h"
#include "Logger.h"
#include "RenderPlane.h"
#include "gtc/matrix_transform.hpp"

class FirstPassProgram : public ShaderProgram {
public:
    ProgramVariable *projection;
    ProgramVariable *modelView;
    ProgramVariable *modelMatrix;
    ProgramVariable *normalMatrix;
    ProgramVariable *shadowMatrix;
    ProgramVariable *texture;
    ProgramVariable *shadowTexture;
    ProgramVariable *diffuseColor;
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

protected:
    virtual void link()
    {
        ShaderProgram::link();
        if (this->isOk())
        {
            this->use();
            this->projection = new ProgramVariable(this, "proj", VariableType::MATRIX4);
            this->modelView = new ProgramVariable(this, "modelView", VariableType::MATRIX4);
            this->modelMatrix = new ProgramVariable(this, "modelMatrix", VariableType::MATRIX4);
            this->shadowMatrix = new ProgramVariable(this, "shadowMvp", VariableType::MATRIX4);
            this->normalMatrix = new ProgramVariable(this, "normalMatrix", VariableType::MATRIX3);
            this->texture = new ProgramVariable(this, "tex", VariableType::TEXTURE);
            this->shadowTexture = new ProgramVariable(this, "shadowTex", VariableType::TEXTURE1);
            this->diffuseColor = new ProgramVariable(this, "color", VariableType::COLOR);
        }
    }
};

class SecondPassProgram : public ShaderProgram {
public:
    ProgramVariable *modelViewProjection;
    ProgramVariable *positionTexture;
    ProgramVariable *normalTexture;
    ProgramVariable *colorTexture;
    ProgramVariable *lightPosition;
    SecondPassProgram()
        : ShaderProgram(), modelViewProjection(nullptr), positionTexture(nullptr),
          normalTexture(nullptr), colorTexture(nullptr), lightPosition(nullptr)
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

protected:
    virtual void link()
    {
        ShaderProgram::link();
        if (this->isOk())
        {
            this->use();
            this->modelViewProjection = new ProgramVariable(this, "mvp", VariableType::MATRIX4);
            this->positionTexture = new ProgramVariable(this, "posTex", VariableType::TEXTURE);
            this->normalTexture = new ProgramVariable(this, "normalTex", VariableType::TEXTURE1);
            this->colorTexture = new ProgramVariable(this, "colorTex", VariableType::TEXTURE2);
            this->lightPosition = new ProgramVariable(this, "lightPos", VariableType::VECTOR3);
        }
    }
};

class ShadowPassProgram : public ShaderProgram
{
public:
    ProgramVariable *shadowMatrix;
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

protected:
    virtual void link()
    {
        ShaderProgram::link();
        if (this->isOk())
        {
            this->use();
            this->shadowMatrix = new ProgramVariable(this, "mvp", VariableType::MATRIX4);
        }
    }
};

RenderScene::RenderScene(const int windowWidth, const int windowHeight)
    : firstPass(nullptr), secondPass(nullptr), shadowPass(nullptr), depthTexture(nullptr),
      windowWidth(windowWidth), windowHeight(windowHeight),
      renderPlane(nullptr), light(nullptr)
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
}

bool RenderScene::initialize()
{
    this->firstPass = new FirstPassProgram();
    this->firstPass->assign(new FragmentShader(std::string("DSFPFragmentShader.glsl")));
    this->firstPass->assign(new VertexShader(std::string("DSFPVertexShader.glsl")));
    if (!this->firstPass->isOk()) {
        return false;
    }

    this->secondPass = new SecondPassProgram();
    this->secondPass->assign(new FragmentShader(std::string("DSSPFragmentShader.glsl")));
    this->secondPass->assign(new VertexShader(std::string("DSSPVertexShader.glsl")));
    if (!this->secondPass->isOk()) {
        return false;
    }

    this->shadowPass = new ShadowPassProgram();
    this->shadowPass->assign(new FragmentShader(std::string("ShadowFragmentShader.glsl")));
    this->shadowPass->assign(new VertexShader(std::string("ShadowVertexShader.glsl")));
    if (!this->shadowPass->isOk()) {
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    GLuint framebuffers[2];
    glGenFramebuffers(2, framebuffers);

    // Shadow pass
    this->shadowFramebuffer = framebuffers[0];
    this->firstPassBuffer = framebuffers[1];

    glBindFramebuffer(GL_FRAMEBUFFER, shadowFramebuffer);
    this->depthTexture = new Texture(1024, 1024, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_FLOAT);
    this->depthTexture->setParameters(GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthTexture->getId(), 0);
    glDrawBuffer(GL_NONE);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Logger::log(std::string("Failed to create shadow framebuffer."));
        return false;
    }

    // First pass
    glBindFramebuffer(GL_FRAMEBUFFER, this->firstPassBuffer);

    this->colorTexture = new Texture(this->textureWidth, this->textureHeight, GL_RGBA32F, GL_RGBA, GL_FLOAT);
    this->colorTexture->setParameters(GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    this->positionTexture = new Texture(this->textureWidth, this->textureHeight, GL_RGBA32F, GL_RGBA, GL_FLOAT);
    this->positionTexture->setParameters(GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    this->normalTexture = new Texture(this->textureWidth, this->textureHeight, GL_RGBA32F, GL_RGBA, GL_FLOAT);
    this->normalTexture->setParameters(GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->positionTexture->getId(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, this->normalTexture->getId(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, this->colorTexture->getId(), 0);

    GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, buffers);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Logger::log(std::string("Failed to create first pass framebuffer."));
        return false;
    }

    glBindFragDataLocation(this->firstPass->getId(), 0, "outPosition");
    glBindFragDataLocation(this->firstPass->getId(), 1, "outNormal");
    glBindFragDataLocation(this->firstPass->getId(), 2, "outColor");

    this->firstPass->projection->setValue(glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 1000.f));

    this->renderPlane = new RenderPlane();

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
    glBindFramebuffer(GL_FRAMEBUFFER, this->shadowFramebuffer);
    glViewport(0, 0, 1024, 1024);
    glClear(GL_DEPTH_BUFFER_BIT);
    this->shadowPass->use();

    const float w = 10.0f;
    const glm::mat4& shadowViewMatrix = glm::ortho(-w, w, -w, w, 0.f, 100.f)
            * glm::lookAt(this->light->getPosition(), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0f, 1.0f, 0.0f));

    for (auto it = this->renderables.cbegin(); it != this->renderables.cend(); it++) {
        const Renderable *m = (Renderable *)*it;
        this->shadowPass->shadowMatrix->setValue(shadowViewMatrix * m->getMatrix());
        m->display();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, this->firstPassBuffer);
    glViewport(0, 0, this->textureWidth, this->textureHeight);
    glClearColor(1.0f, 0.1f, 0.5f, 1.0f);
    glViewport(0, 0, this->windowWidth, this->windowHeight);
    glClearColor(0.0f, 0.1f, 0.5f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::mat4 biasMatrix(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0);

    const glm::mat4& view = this->camera->getMatrix();

    for (auto it = this->renderables.cbegin(); it != this->renderables.cend(); it++) {
        const Renderable *m = (Renderable *)*it;
        const glm::mat4& model = m->getMatrix();
        const glm::mat4& modelView = view * model;
        this->firstPass->use();
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
        this->firstPass->shadowTexture->setValue(this->depthTexture);

        m->display();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, this->windowWidth, this->windowHeight);

    glClearColor(1.0f, 0.1f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const float planeX = (this->textureWidth - this->windowWidth)/2.f;
    const float planeY = (this->textureHeight - this->windowHeight)/2.f;
    const glm::mat4 planeProjection = glm::ortho(-this->windowWidth/2.f, this->windowWidth/2.f, -this->windowHeight/2.f, this->windowHeight/2.f, -1.f, 1000.f);
    const glm::mat4& spMvp = planeProjection *
                glm::scale(glm::translate(glm::mat4(), glm::vec3(planeX, planeY, 1.f)), glm::vec3(this->textureWidth/2.f, this->textureHeight/2.f, -1));

    this->secondPass->use();
    this->secondPass->lightPosition->setValue(this->light->getPosition());
    this->secondPass->modelViewProjection->setValue(spMvp);
    this->renderPlane->display();
}
