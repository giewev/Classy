#include <string>
#include "Logger.h"
#include "spdlog/spdlog.h"

Logger Logger::mainLogInstance;

Logger::Logger()
{
    const int logSizeMb = 5;
    const int mbSize = 1048576;
    const int fileCount = 3;
    this->log = spdlog::rotating_logger_mt("MainLogger", "Classy", logSizeMb * mbSize, fileCount);
}

Logger Logger::mainLog()
{
    return this->mainLogInstance;
}

void Logger::writeInfo(std::string message)
{
    this->log->info(message);
}
