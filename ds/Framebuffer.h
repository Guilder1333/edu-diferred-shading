#pragma once

#include <vector>
#include <GL/glew.h>

class Framebuffer
{
public:
    Framebuffer(const int width, const int height);
    ~Framebuffer();
    const class Texture *addBuffer(const int iternalFormat, const int format, const int componentFormat, const GLenum attachment);
    bool initialize();
    void bind() const;
    static void bindMain();
    GLint getId() const;
private:
    std::vector<struct Buffer> buffers;
    GLuint id;
    GLint width;
    GLint height;
};
