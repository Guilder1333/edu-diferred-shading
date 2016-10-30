#pragma once

#include <glm.hpp>

enum class VariableType {
    TEXTURE,
    TEXTURE1,
    TEXTURE2,
    TEXTURE3,
    COLOR,
    INTEGER,
    FLOAT,
    VECTOR2,
    VECTOR3,
    VECTOR4,
    MATRIX3,
    MATRIX4
};

class ProgramVariable
{
public:
    ProgramVariable(const class ShaderProgram *program, const char *name, const VariableType type);
    ProgramVariable(const int id, const VariableType type);
    void setValue(const class Texture *);
    void setValue(const glm::mat4 mat);
    void setValue(const glm::mat3 mat);
    void setValue(const glm::vec2 vec);
    void setValue(const glm::vec3 vec);
    void setValue(const glm::vec4 vec);
    void setValue(const int value);
    void setValue(const float value);
    bool isOk() const;
private:
    int id;
    VariableType type;
};
