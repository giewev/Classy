#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include "spdlog/spdlog.h"

class Logger
{
public:
    void writeInfo(std::string message);
    static std::shared_ptr<spdlog::logger> mainLog();
protected:
private:
    std::shared_ptr<spdlog::logger> log;
    static Logger mainLogInstance;
    Logger();
};

#endif // LOGGER_H
