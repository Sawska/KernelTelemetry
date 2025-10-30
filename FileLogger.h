#ifndef  FILE_LOGGER_H
#define FILE_LOGGER_H


#include "Metrics.h"
#include <string>
#include <fstream>

class FileLogger {
public:
FileLogger(const std::string& logFilePath);

~FileLogger();

void log(const SystemMetrics& metrics);

private:

    std::ofstream logFile;
};

#endif //FILE_LOGGER_H

