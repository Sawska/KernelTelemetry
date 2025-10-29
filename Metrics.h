#ifndef  METRICS_H

#define METRICS_H
#include <string>
#include <vector>

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

// /proc/diskstats
struct DiskStats {
    std::string deviceName;
    double readsPerSecond = 0.0;
    double writesPerSecond = 0.0;
    double megabytesWrittenPerSecond = 0.0;
};

// /proc/net/dev
struct NetworkStats {
    std::string interfaceName;
    double megabytesReceivedPerSecond = 0.0;
    double megabytesSentPerSecond = 0.0;
    double packetsREceivedPerSecond = 0.0;
    double packetsSendPerSecond = 0.0;
};


struct SystemMetrics {
    long long timestamp = 0;
    MemInfo memory;
    CpuStats cpu;
    std::vector<DiskStats> disks;
    std::vector<NetworkStats> network;
};

#endif //METRICS_H