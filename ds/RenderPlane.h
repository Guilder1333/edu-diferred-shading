#pragma once

#include "Renderable.h"

class RenderPlane : public Renderable
{
public:
    RenderPlane();
    virtual ~RenderPlane();

    void display() const;
private:
    glm::vec4 color;
    glm::mat4 mx;
    unsigned int buffer;
    unsigned int vao;
};
