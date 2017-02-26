#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include "spdlog/spdlog.h"

class Logger
{
    public:
        void writeInfo(std::string message);
        Logger mainLog();
    protected:
    private:
    static Logger mainLogInstance;
    std::shared_ptr<spdlog::logger> log;
    Logger();
};

#endif // LOGGER_H
