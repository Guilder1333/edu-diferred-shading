#pragma once

#include "Renderable.h"

class Camera : public Renderable
{
public:
    Camera();
    virtual ~Camera();

    void setPerspective(const float fovy, const float aspect, const float near, const float far);
    void setOrtho(const float left, const float right, const float bottom, const float top, const float near, const float far);

    const glm::mat4 &getProjection() const;
    virtual void display() const;
private:
    glm::mat4 projection;
};
