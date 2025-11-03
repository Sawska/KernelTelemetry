#include "TelemetryCollector.h"
#include "FileLogger.h"
#include "Metrics.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <exception>

int main() {
    try {
        TelemetryCollector collector;
        
        FileLogger logger("/var/log/kernel_telemetry/app.log"); 

        while (true) {
            SystemMetrics metrics = collector.getAllStats();

            logger.log(metrics);

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR (std::exception): " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "FATAL ERROR: An unknown, unhandled exception occurred." << std::endl;
        return 1;
    }
    
    return 0;
}