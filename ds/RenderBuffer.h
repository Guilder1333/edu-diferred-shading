#pragma once

#include <GL/glew.h>

class RenderBuffer
{
public:
    RenderBuffer(const GLsizei width, const GLsizei height, const GLenum internalFormat);
    ~RenderBuffer();
    GLuint getId() const;
    bool isOk() const;
private:
    GLuint id;
};
