#pragma once
#include <glm.hpp>
#include <GL/glew.h>
#include "Material.h"

class Renderable
{
public:
    Renderable();
    Renderable(const Renderable *parent);
	virtual ~Renderable();

    void setPosition(const GLfloat x, const GLfloat y, const GLfloat z);
    void setPosition(const glm::vec3 &position);
    void setRotation(const GLfloat pitch, const GLfloat yaw, const GLfloat roll);
    const glm::vec3 &getPosition() const;
    glm::mat4 getMatrix() const;

    virtual void display() const = 0;
    virtual void beforeDisplay() const;
    void setMaterial(const Material *material);
    const Material *getMaterial() const;
private:
    glm::vec3 position;
    GLfloat pitch;
    GLfloat yaw;
    GLfloat roll;
    const Material *material;
    const Renderable *parent;
};

