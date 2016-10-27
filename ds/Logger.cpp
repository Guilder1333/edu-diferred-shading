#include "Logger.h"

#include <GL/glew.h>
#include <iostream>

static Logger *instance;

Logger::Logger(const std::string &fileName)
    : out(fileName)
{
}

Logger::~Logger()
{
    this->out.close();
}

void Logger::logMessage(const std::string &message)
{
    this->out << message << std::endl;
    std::cout << message << std::endl;
}

void Logger::logProgramMessage(const int program)
{
    int maxLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
    if (maxLength == 0) {
        maxLength = 1;
    }
    int length = 0;
    char *log = new char[maxLength];
    glGetProgramInfoLog(program, maxLength, &length, log);
    log[length] = 0;
    Logger::log(std::string("program info log for GL index ") + std::to_string(program) + ":\n" + log);
    delete[]log;
}

Logger *Logger::instance()
{
    return ::instance;
}

void Logger::log(const std::string &message)
{
    instance()->logMessage(message);
}

void Logger::logProgram(const int program)
{
    instance()->logProgramMessage(program);
}

void Logger::initialize(const std::string &fileName)
{
    ::instance = new Logger(fileName);
}

void Logger::destroy()
{
    delete ::instance;
}
