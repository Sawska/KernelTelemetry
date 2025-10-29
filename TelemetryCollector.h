#ifndef TELEMETRY_COLLECTOR_H

#define TELEMETRY_COLLECTOR_H


#include <vector>
#include "Metrics.h"






class TelemetryCollector {
public:
    SystemMetrics getAllStats();
private:
    MemInfo parse_meminfo();
    CpuStats parse_cpu();
    std::vector<DiskStats> parse_diskstats();
    std::vector<NetworkStats> parse_net_dev();
};

#endif //TELEMETRY_COLLECTOR_H