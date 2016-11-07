#include "Framebuffer.h"

#include "Texture.h"
#include "RenderBuffer.h"

struct Buffer {
    GLenum attachment;
    Texture *texture;
    RenderBuffer *renderBuffer;
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
        if (buffer.texture != nullptr) {
            delete buffer.texture;
        }
        if (buffer.renderBuffer != nullptr) {
            delete buffer.renderBuffer;
        }
    }
}

const Texture *Framebuffer::addBuffer(const GLenum internalFormat, const GLenum format, const GLenum componentFormat, const GLenum attachment)
{
    Texture *texture = new Texture(this->width, this->height, internalFormat, format, componentFormat);
    texture->setParameters(GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    Buffer buffer = { attachment, texture, 0 };
    this->buffers.push_back(buffer);
    return texture;
}

const RenderBuffer *Framebuffer::addBuffer(const GLenum internalFormat, const GLenum attachment)
{
    RenderBuffer *renderBuffer = new RenderBuffer(this->width, this->height, internalFormat);

    Buffer buffer = { attachment, 0, renderBuffer };
    this->buffers.push_back(buffer);
    return renderBuffer;
}

bool Framebuffer::initialize()
{
    glCreateFramebuffers(1, &this->id);
    this->bind();

    GLenum *attachments = new GLenum[this->buffers.size()];
    int index = 0;
    for(auto it = this->buffers.cbegin(); it != this->buffers.cend(); it++) {
        const Buffer &buffer = (Buffer)*it;
        if (buffer.renderBuffer != nullptr) {
            if (!buffer.renderBuffer->isOk()) {
                return false;
            }
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, buffer.attachment, GL_RENDERBUFFER, buffer.renderBuffer->getId());
        } else {
            if (!buffer.texture->isOk()) {
                return false;
            }
            glFramebufferTexture2D(GL_FRAMEBUFFER, buffer.attachment, GL_TEXTURE_2D, buffer.texture->getId(), 0);
        }
        if (buffer.attachment != GL_DEPTH_ATTACHMENT) {
            attachments[index++] = buffer.attachment;
        }
    }
    if (index == 0) {
        glDrawBuffer(GL_NONE);
    } else {
        glDrawBuffers(index, attachments);
    }
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
