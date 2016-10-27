#include "Mesh.h"

Mesh::Mesh(const Mesh *m)
    : Mesh(m->vao, 0, m->points)
{
}

Mesh::Mesh(const GLuint vao, const GLuint buffer, const GLint points)
    : Renderable(), vao(vao), buffer(buffer), points(points)
{

}

Mesh::Mesh(const GLuint vao, const GLuint buffer, const GLint points, const Renderable *parent)
    : Renderable(parent), vao(vao), buffer(buffer), points(points)
{

}

Mesh::~Mesh()
{
    if (this->buffer != 0) {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &buffer);
    }
}

void Mesh::display() const
{
    glBindVertexArray(this->vao);
	glDrawArrays(GL_TRIANGLES, 0, this->points);
}


