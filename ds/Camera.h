#pragma once

#include "Renderable.h"

class Camera : public Renderable
{
public:
    Camera();
    virtual ~Camera();

    void setPerspective(const GLfloat fovy, const GLfloat aspect, const GLfloat near, const GLfloat far);
    void setOrtho(const GLfloat left, const GLfloat right, const GLfloat bottom, const GLfloat top, const GLfloat near, const GLfloat far);

    const glm::mat4 getProjection() const;
    virtual void display() const;
private:
    glm::mat4 projection;
};
