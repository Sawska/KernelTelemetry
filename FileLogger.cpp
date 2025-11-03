#include "FileLogger.h"
#include <nlohmann/json.hpp>
#include <iostream>

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MemInfo, memTotal, memAvailable, swapTotal, swapFree)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CpuStats, usagePercentage, iowaitPercentage)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DiskStats, deviceName, readsPerSecond, writesPerSecond, megabytesReadPerSecond, megabytesWrittenPerSecond)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(NetworkStats, interfaceName, megabytesReceivedPerSecond, megabytesSentPerSecond, packetsReceivedPerSecond, packetsSentPerSecond)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SystemMetrics, memory, cpu, disks, network)

FileLogger::~FileLogger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

FileLogger::FileLogger(const std::string& logFilePath) {
    logFile.open(logFilePath, std::ios::out | std::ios::app);
    
    if (!logFile.is_open()) {
        throw std::runtime_error("FATAL: Could not open log file: " + logFilePath + ". Check permissions!");
    }
}
void FileLogger::log(const SystemMetrics& metrics) {
    if (!logFile.is_open()) {
        return;
    }

    nlohmann::json metricsJson = metrics;

    logFile << metricsJson.dump() << std::endl;
}