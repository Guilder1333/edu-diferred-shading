#include "Triangle.h"

#include <GL/glew.h>

static GLfloat gl_triagnle_vertices[] = {
	0.0f,  0.5f,  0.0f,
	0.5f, -0.5f,  0.0f,
	-0.5f, -0.5f,  0.0f
};

Triangle::Triangle()
{
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gl_triagnle_vertices), gl_triagnle_vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
}


Triangle::~Triangle()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &buffer);
}

void Triangle::display()
{
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, sizeof(gl_triagnle_vertices) / (sizeof(GLfloat) * 3));

}
