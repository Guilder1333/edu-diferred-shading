#pragma once

#include "Renderable.h"

class RenderPlane : public Renderable
{
public:
    RenderPlane();
    virtual ~RenderPlane();

    void beforeDisplay() const;
    void display() const;
private:
    unsigned int buffer;
    unsigned int vao;
};
