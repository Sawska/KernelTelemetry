#include "TelemetryCollector.h"
#include <vector>
#include <chrono>

MemInfo TelemetryCollector::parse_meminfo() {
    MemInfo mem;


    return mem;
}

CpuStats TelemetryCollector::parse_cpu() {
    CpuStats cpu;

    return cpu;
}

std::vector<DiskStats> TelemetryCollector::parse_diskstats() {
    std::vector<DiskStats> allDisks;


    return allDisks;
}

std::vector<NetworkStats> TelemetryCollector::parse_net_dev() {
    std::vector<NetworkStats> allInterfaces;

    return allInterfaces;
}




SystemMetrics TelemetryCollector::getAllStats() {
    SystemMetrics metrics;

    auto now = std::chrono::system_clock::now();
    metrics.timestamp = std::chrono::system_clock::to_time_t(now);

    metrics.memory = parse_meminfo();

    metrics.cpu = parse_cpu();

    metrics.disks = parse_diskstats();

    metrics.network = parse_net_dev();

    return metrics;
}