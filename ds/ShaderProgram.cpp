#include "ShaderProgram.h"

#include <GL/glew.h>
#include "VertexShader.h"
#include "FragmentShader.h"
#include "Logger.h"

ShaderProgram::ShaderProgram()
    : vertexShader(nullptr),
      fragmentShader(nullptr),
      linked(false),
      program(glCreateProgram())
{

}

ShaderProgram::~ShaderProgram()
{
    if (this->program != 0) {
        glDeleteProgram(this->program);
    }
    if (this->vertexShader != nullptr) {
        delete this->vertexShader;
    }
    if (this->fragmentShader != nullptr) {
        delete this->fragmentShader;
    }
}

void ShaderProgram::assign(const VertexShader *shader)
{
    glAttachShader(this->program, shader->getShader());
    this->vertexShader = shader;
    if (this->fragmentShader != nullptr) {
        link();
    }
}

void ShaderProgram::assign(const FragmentShader *shader)
{
    glAttachShader(this->program, shader->getShader());
    this->fragmentShader = shader;
    if (this->vertexShader != nullptr) {
        link();
    }
}

bool ShaderProgram::isOk() const
{
    return this->program != 0 && this->linked;
}

void ShaderProgram::use() const
{
    static const ShaderProgram *current;
    if (current != this) {
        current = this;
        glUseProgram(this->program);
    }
}

unsigned int ShaderProgram::getId() const
{
    return this->program;
}

void ShaderProgram::link()
{
    glLinkProgram(this->program);
    GLint params;
    glGetProgramiv(this->program, GL_LINK_STATUS, &params);
    this->linked = params == GL_TRUE;
    if (!this->linked) {
        Logger::log(std::string("Failed to link program\n"));
        Logger::logProgram(this->program);
    }
}
