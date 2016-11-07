#include "Shader.h"
#include "Logger.h"
#include <fstream>
#include <GL/glew.h>
#include <string>

Shader::Shader(const std::string& fileName, GLuint shaderType)
    : type(shaderType), shader(0)
{
    std::ifstream f;
    f.open(fileName, std::ifstream::binary | std::ifstream::ate);
    if (!f.is_open()) {
        Logger::log(std::string("Shader file '") + fileName + "'' not found.\n");
        return;
    }
    std::streamoff length = f.tellg();
    f.seekg(std::ifstream::beg);
    char *str = new char[length + 1];
    f.read(str, length);
    str[length] = 0;
    f.close();

//#if QT_DEBUG
    GLint params;
    // somehow it has better error check
    GLuint program = glCreateShaderProgramv(shaderType, 1, &str);
    glGetProgramiv(program, GL_LINK_STATUS, &params);
    if (params != GL_TRUE) {
        Logger::log(std::string("Failed to link vertex shader program"));
        Logger::logProgram(program);
        glDeleteProgram(program);
        return;
    }
    glDeleteProgram(program);
//#endif

    const GLuint shader = glCreateShader(shaderType);
    if (shader != 0) {
        glShaderSource(shader, 1, &str, NULL);
        glCompileShader(shader);
    }

    delete[]str;
    this->shader = shader;
}

Shader::~Shader()
{
    if (this->shader != 0) {
        glDeleteShader(this->shader);
    }
}

GLuint Shader::getType() const
{
    return this->type;
}

GLuint Shader::getShader() const
{
    return this->shader;
}

bool Shader::isOk() const
{
    return this->shader != 0;
}

bool Shader::isVertexShader() const
{
    return this->type == GL_VERTEX_SHADER;
}
