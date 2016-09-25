#pragma once

#include "Renderable.h"

class Plane : public Renderable
{
public:
    Plane();
    ~Plane();

    virtual void setMatrix(glm::mat4 &);
    virtual glm::mat4 getMatrix();
    virtual glm::vec4 diffuseColor();

    void display();
private:
    glm::vec4 color;
    glm::mat4 mx;
    unsigned int buffer;
    unsigned int vao;
};
