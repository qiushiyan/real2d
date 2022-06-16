#ifndef LOGGER_H
#define LOGGER_H
#include <string>
#include <string_view>
class Logger
{
private:
public:
    static void info(std::string_view message);
    static void error(std::string_view message);
};

#endif