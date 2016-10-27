#include "VertexShader.h"

#include <GL/glew.h>

VertexShader::VertexShader(const std::string &fileName)
    : Shader(fileName, GL_VERTEX_SHADER)
{

}
