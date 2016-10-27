#pragma once

#include <fstream>
#include <iostream>
#include <string>

class Logger
{
public:
    Logger(const std::string &fileName);
    ~Logger();

    void logMessage(const std::string &message);
    void logProgramMessage(const int program);

    static Logger *instance();
    static void log(const std::string &message);
    static void logProgram(const int program);
    static void initialize(const std::string &fileName);
    static void destroy();
private:

    std::ofstream out;
};
