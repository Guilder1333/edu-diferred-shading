#include "RenderBuffer.h"

RenderBuffer::RenderBuffer(const GLsizei width, const GLsizei height, const GLenum internalFormat)
{
    glGenRenderbuffers(1, &this->id);
    glBindRenderbuffer(GL_RENDERBUFFER, this->id);
    glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
}

RenderBuffer::~RenderBuffer()
{
    if (this->id != 0) {
        glDeleteRenderbuffers(1, &this->id);
    }
}

GLuint RenderBuffer::getId() const
{
    return this->id;
}

bool RenderBuffer::isOk() const
{
    return this->id != 0;
}
