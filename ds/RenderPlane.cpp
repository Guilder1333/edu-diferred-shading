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
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    this->mx = glm::mat4();
    this->color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
}

RenderPlane::~RenderPlane()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &buffer);
}

void RenderPlane::setMatrix(glm::mat4 &mx)
{
    this->mx = mx;
}

glm::mat4 RenderPlane::getMatrix()
{
    return mx;
}

glm::vec4 RenderPlane::diffuseColor()
{
    return color;
}

void RenderPlane::display()
{
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
