#pragma once

#include "Renderable.h"

class Cube : public Renderable
{
public:
	Cube();
	~Cube();

    void display() const;
private:
    GLuint buffer;
    GLuint vao;
};

