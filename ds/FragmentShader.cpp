#include "FragmentShader.h"

#include <GL/glew.h>

FragmentShader::FragmentShader(const std::string &fileName)
    : Shader(fileName, GL_FRAGMENT_SHADER)
{

}
