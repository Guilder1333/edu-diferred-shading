#include "ProgramVariable.h"
#include <gtc/type_ptr.hpp>
#include <GL/glew.h>
#include "Texture.h"
#include "ShaderProgram.h"

ProgramVariable::ProgramVariable(const ShaderProgram *program, const char *name, const VariableType type)
    : ProgramVariable(glGetUniformLocation(program->getId(), name), type)
{
}

ProgramVariable::ProgramVariable(const int id, const VariableType type)
    : id(id), type(type)
{
    switch(this->type) {
    case VariableType::TEXTURE:
        glUniform1d(this->id, 0);
        break;
    case VariableType::TEXTURE1:
        glUniform1d(this->id, 1);
        break;
    case VariableType::TEXTURE2:
        glUniform1d(this->id, 2);
        break;
    case VariableType::TEXTURE3:
        glUniform1d(this->id, 3);
        break;
    }
}

void ProgramVariable::setValue(const Texture *texture)
{
    // TODO: can be made with templates, to avoid runtime checks.
    switch(this->type) {
    case VariableType::TEXTURE:
        glActiveTexture(GL_TEXTURE0);
        break;
    case VariableType::TEXTURE1:
        glActiveTexture(GL_TEXTURE1);
        break;
    case VariableType::TEXTURE2:
        glActiveTexture(GL_TEXTURE2);
        break;
    case VariableType::TEXTURE3:
        glActiveTexture(GL_TEXTURE3);
        break;
    }
    if (texture == nullptr) {
        glBindTexture(GL_TEXTURE_2D, 0);
    } else {
        glBindTexture(GL_TEXTURE_2D, texture->getId());
    }
}

void ProgramVariable::setValue(const glm::mat4 &mat)
{
    glUniform2fv(this->id, 1, glm::value_ptr(mat));
}

void ProgramVariable::setValue(const glm::mat3 &mat)
{
    glUniform2fv(this->id, 1, glm::value_ptr(mat));
}

void ProgramVariable::setValue(const glm::vec2 &vec)
{
    glUniform2fv(this->id, 1, glm::value_ptr(vec));
}

void ProgramVariable::setValue(const glm::vec3 &vec)
{
    glUniform3fv(this->id, 1, glm::value_ptr(vec));
}

void ProgramVariable::setValue(const glm::vec4 &vec)
{
    glUniform4fv(this->id, 1, glm::value_ptr(vec));
}

void ProgramVariable::setValue(const int value)
{
    glUniform1d(this->id, value);
}

void ProgramVariable::setValue(const float value)
{
    glUniform1f(this->id, value);
}

bool ProgramVariable::isOk() const
{
    return this->id >= 0;
}
