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
    if (!shader->isOk()) {
        return;
    }

    glAttachShader(this->program, shader->getShader());
    this->vertexShader = shader;
    if (this->fragmentShader != nullptr) {
        link();
    }
}

void ShaderProgram::assign(const FragmentShader *shader)
{
    if (!shader->isOk()) {
        return;
    }

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
    ShaderProgram::useProgram(this);
}

unsigned int ShaderProgram::getId() const
{
    return this->program;
}

void ShaderProgram::useProgram(const ShaderProgram *program)
{
    static const ShaderProgram *current;
    if (current != program) {
        current = program;
        if (program == nullptr) {
            glUseProgram(0);
        } else {
            glUseProgram(program->getId());
        }
    }
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
