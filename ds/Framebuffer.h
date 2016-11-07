#pragma once

#include "RenderBuffer.h"
#include "Texture.h"

#include <GL/glew.h>

#include <vector>

class Framebuffer
{
public:
    Framebuffer(const GLsizei width, const GLsizei height);
    ~Framebuffer();
    const Texture *addBuffer(const GLenum internalFormat, const GLenum format, const GLenum componentFormat, const GLenum attachment);
    const RenderBuffer *addBuffer(const GLenum internalFormat, const GLenum attachment);
    bool initialize();
    void bind() const;
    static void bindMain();
    GLint getId() const;
private:
    std::vector<struct Buffer> buffers;
    GLuint id;
    GLsizei width;
    GLsizei height;
};
