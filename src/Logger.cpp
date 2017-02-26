#include "Logger.h"
#include "spdlog/spdlog.h"

Logger::Logger()
{
    int logSizeMb = 5;
    int mbSize = 1048576;
    int fileCount = 3;
    this->log = spdlog::rotating_logger_mt("MainLogger", "Classy.log", logSizeMb * mbSize, fileCount);
    for (int i = 0; i < 10; ++i)
            this->log->info("{} * {} equals {:>10}", i, i, i*i);
}
