#ifndef METRICS_H
#define METRICS_H

#include <string>
#include <vector>
#include <ctime> // For time_t / long long timestamp

// /proc/meminfo
struct MemInfo {
    long long memTotal = 0;
    long long memAvailable = 0;
    long long swapTotal = 0;
    long long swapFree = 0;
};

// /proc/stat
struct CpuStats {
    double usagePercentage = 0.0;
    double iowaitPercentage = 0.0;
};

// /proc/diskstats (FIXED: Added megabytesReadPerSecond)
struct DiskStats {
    std::string deviceName;
    double readsPerSecond = 0.0;
    double writesPerSecond = 0.0;
    double megabytesReadPerSecond = 0.0;   // <-- FIX 1
    double megabytesWrittenPerSecond = 0.0;
};

// /proc/net/dev (Assuming the provided names are correct, just check your macro!)
struct NetworkStats {
    std::string interfaceName;
    double megabytesReceivedPerSecond = 0.0;
    double megabytesSentPerSecond = 0.0;
    double packetsReceivedPerSecond = 0.0;
    double packetsSentPerSecond = 0.0;
};


// Top-level container
struct SystemMetrics {
    long long timestamp = 0;
    MemInfo memory;
    CpuStats cpu;
    std::vector<DiskStats> disks;
    std::vector<NetworkStats> network;
};

#endif //METRICS_H