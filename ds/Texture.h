#pragma once

#include <GL/glew.h>

#include <string>

class Texture
{
public:
    Texture(const std::string &fileName);
    Texture(const GLsizei width, const GLsizei height, const GLenum iternalFormat, const GLenum format, const GLenum componentFormat);
    ~Texture();

    void setParameters(const GLint magFilter, const GLint minFilter, const GLint wrapS, const GLint wrapT);

    bool isOk() const;
    const std::string &getFileName() const;
    GLuint getId() const;
    void use() const;
    static void unbind(const GLint index);
private:
    GLuint texture;
    std::string fileName;
};
