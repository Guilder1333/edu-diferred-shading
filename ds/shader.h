#pragma once

#include <GL/glew.h>

#include <string>

class Shader
{
public:
    virtual ~Shader();
    GLuint getType() const;
    GLuint getShader() const;
    bool isOk() const;
    bool isVertexShader() const;
protected:
    Shader(const std::string& fileName, const GLuint shaderType);
private:
    GLuint shader;
    GLuint type;
};
