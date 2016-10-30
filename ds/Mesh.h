#pragma once
#include <glm.hpp>
#include "Renderable.h"
#include "lib3ds/types.h"

class Mesh : public Renderable
{
public:
    Mesh(const Mesh *);
    Mesh(const GLuint vao, const GLuint buffer, const GLint points);
    Mesh(const GLuint vao, const GLuint buffer, const GLint points, const Renderable *parent);
    virtual ~Mesh();

    void beforeDisplay() const;
    void display() const;
private:
    GLuint buffer;
    GLuint vao;
    GLint points;
};

