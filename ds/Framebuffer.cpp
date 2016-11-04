#include "Framebuffer.h"

#include "Texture.h"

struct Buffer {
    GLenum attachment;
    Texture *texture;
};

Framebuffer::Framebuffer(const int width, const int height)
    : id(0)
{
    this->width = width;
    this->height = height;
}

Framebuffer::~Framebuffer()
{
    if (this->id != 0) {
        glDeleteFramebuffers(1, &this->id);
    }
    for(auto it = this->buffers.cbegin(); it != this->buffers.cend(); it++) {
        const Buffer &buffer = (Buffer)*it;
        delete buffer.texture;
    }
}

const Texture *Framebuffer::addBuffer(const int internalFormat, const int format, const int componentFormat, const GLenum attachment)
{
    Texture *texture = new Texture(this->width, this->height, internalFormat, format, componentFormat);
    texture->setParameters(GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    Buffer buffer = { attachment, texture };
    this->buffers.push_back(buffer);
    return texture;
}

bool Framebuffer::initialize()
{
    glCreateFramebuffers(1, &this->id);
    this->bind();

    GLenum *attachments = new GLenum[this->buffers.size()];
    int index = 0;
    for(auto it = this->buffers.cbegin(); it != this->buffers.cend(); it++) {
        const Buffer &buffer = (Buffer)*it;
        glFramebufferTexture2D(GL_FRAMEBUFFER, buffer.attachment, GL_TEXTURE_2D, buffer.texture->getId(), 0);
        attachments[index++] = buffer.attachment;
    }
    glDrawBuffers(this->buffers.size(), attachments);
    delete []attachments;

    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void Framebuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->id);
}

void Framebuffer::bindMain()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
