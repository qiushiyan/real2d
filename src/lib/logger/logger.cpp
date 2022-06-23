#include <Logger.hpp>
#include <string>
#include <string_view>
#include <iostream>
#include <chrono>
#include <ctime>

std::string CurrentDateTimeToString()
{
    using namespace std::chrono;
    auto now{system_clock::to_time_t(system_clock::now())};
    std::string output(30, '\0');
    std::strftime(&output[0], output.size(), "%d-%b-%Y %H:%M:%S", std::localtime(&now));
    return output;
}

void Logger::info(std::string_view message)
{
    std::cout << "\x1B[32m"
              << "[INFO " << CurrentDateTimeToString() + "] " << message << "\033[0m" << std::endl;
}

void Logger::error(std::string_view message)
{
    std::cerr << "\x1B[91m"
              << "[ERROR " << CurrentDateTimeToString() + "] " << message << "\033[0m" << std::endl;
}