#pragma once

#include "Renderable.h"

class RenderPlane : public Renderable
{
public:
    RenderPlane();
    virtual ~RenderPlane();

    void display() const;
private:
    GLuint buffer;
    GLuint vao;
};
