#include "RenderPlane.h"
#include <GL/glew.h>

static const GLfloat g_quad_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f,  1.0f, 0.0f,
};


RenderPlane::RenderPlane()
{
    glGenBuffers(1, &this->buffer);
    glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

RenderPlane::~RenderPlane()
{
    glDeleteVertexArrays(1, &this->vao);
    glDeleteBuffers(1, &this->buffer);
}

void RenderPlane::beforeDisplay() const
{
    glBindVertexArray(this->vao);
}

void RenderPlane::display() const
{
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
