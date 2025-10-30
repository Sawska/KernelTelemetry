#include "TelemetryCollector.h"
#include "FileLogger.h"
#include <thread>
#include <chrono>


int main() {
    TelemetryCollector collector;
    FileLogger logger("system_telemetry.log"); 

    while (true) {
        SystemMetrics metrics = collector.getAllStats();

        logger.log(metrics);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}