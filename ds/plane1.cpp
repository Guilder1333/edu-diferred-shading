#include "Plane.h"

#include <GL/glew.h>

static const GLfloat g_quad_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f,  1.0f, 0.0f,
};


Plane::Plane()
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

Plane::~Mesh()
{
}


void Plane::setMatrix(glm::mat4 &mx)
{
    this->mx = mx;
}

glm::mat4 Plane::getMatrix()
{
    if (parent == nullptr) {
        return mx;
    }
    return parent->getMatrix() * mx;
}

glm::vec4 Plane::diffuseColor()
{
    return color;
}

void Plane::display()
{
    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindSampler(0, sampler);
    glDrawArrays(GL_TRIANGLES, 0, this->points);
}
