#ifndef LOGGER_H
#define LOGGER_H
#include "spdlog/spdlog.h"

class Logger
{
    public:
        Logger();
    protected:
    private:
    std::shared_ptr<spdlog::logger> log;
};

#endif // LOGGER_H
