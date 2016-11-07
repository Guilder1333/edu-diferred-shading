#pragma once

#include "Texture.h"
#include "ShaderProgram.h"

#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <GL/glew.h>

template<typename T, GLint TextureIndex = 0>
class ProgramVariable
{
public:
    ProgramVariable(const ShaderProgram *program, const GLchar *name)
        : ProgramVariable(glGetUniformLocation(program->getId(), name))
    {

    }

    ProgramVariable(const GLint id)
        : id(id)
    {
        if (isOk()) {
            glUniform1i(this->id, TextureIndex);
        }
    }

    void setValue(const T *value)
    {
        this->setValueInternal(value);
    }

    void setValue(const T &value)
    {
        this->setValueInternal(value);
    }

    bool isOk() const
    {
        return this->id >= 0;
    }
private:
    inline void setValueInternal(const Texture *value)
    {
        glActiveTexture(GL_TEXTURE0 + TextureIndex);
        if (value == nullptr) {
            glBindTexture(GL_TEXTURE_2D, 0);
        } else {
            value->use();
        }
    }
    inline void setValueInternal(const glm::mat4 *mat)
    {
        glUniformMatrix4fv(this->id, 1, GL_FALSE, mat);
    }
    inline void setValueInternal(const glm::mat3 *mat)
    {
        glUniformMatrix3fv(this->id, 1, GL_FALSE, mat);
    }
    inline void setValueInternal(const glm::vec2 *vec)
    {
        glUniform2fv(this->id, 1, vec);
    }

    inline void setValueInternal(const glm::vec3 *vec)
    {
        glUniform3fv(this->id, 1, vec);
    }
    inline void setValueInternal(const glm::vec4 *vec)
    {
        glUniform4fv(this->id, 1, vec);
    }

    inline void setValueInternal(const GLint *value)
    {
        glUniform1i(this->id, *value);
    }
    inline void setValueInternal(const GLfloat *value)
    {
        glUniform1f(this->id, *value);
    }

    inline void setValueInternal(const Texture &value)
    {
        glActiveTexture(GL_TEXTURE0 + TextureIndex);
        value.use();
    }
    inline void setValueInternal(const glm::mat4 &mat)
    {
        glUniformMatrix4fv(this->id, 1, GL_FALSE, glm::value_ptr(mat));
    }
    inline void setValueInternal(const glm::mat3 &mat)
    {
        glUniformMatrix3fv(this->id, 1, GL_FALSE, glm::value_ptr(mat));
    }
    inline void setValueInternal(const glm::vec2 &vec)
    {
        glUniform2fv(this->id, 1, glm::value_ptr(vec));
    }
    inline void setValueInternal(const glm::vec3 &vec)
    {
        glUniform3fv(this->id, 1, glm::value_ptr(vec));
    }
    inline void setValueInternal(const glm::vec4 &vec)
    {
        glUniform4fv(this->id, 1, glm::value_ptr(vec));
    }
    inline void setValueInternal(const GLint &value)
    {
        glUniform1i(this->id, value);
    }
    inline void setValueInternal(const GLfloat &value)
    {
        glUniform1f(this->id, value);
    }

    GLint id;
};
