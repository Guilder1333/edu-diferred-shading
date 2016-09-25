#pragma once
#include <glm.hpp>

class Renderable
{
public:
    Renderable();
	virtual ~Renderable();

	virtual void setMatrix(glm::mat4 &) = 0;
    virtual glm::mat4 getMatrix() = 0;
    virtual glm::vec4 diffuseColor() = 0;
	virtual void display() = 0;
};

