#pragma once

#include <string>

class Shader
{
public:
    virtual ~Shader();
    unsigned int getType() const;
    unsigned int getShader() const;
    bool isOk() const;
    bool isVertexShader() const;
protected:
    Shader(const std::string& fileName, const unsigned int shaderType);
private:
    unsigned int shader;
    unsigned int type;
};
