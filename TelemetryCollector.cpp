#include "TelemetryCollector.h"
#include "Metrics.h"
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <chrono>

constexpr double SECTOR_SIZE_BYTES = 512.0;


std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << path << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}



MemInfo TelemetryCollector::parse_meminfo() {
    MemInfo mem;

    std::string content = readFile("/proc/meminfo");

    if (content.empty()) {
        return mem;
    }

    std::stringstream ss(content);
    std::string line;

    while (std::getline(ss,line)) {
        std::stringstream line_stream(line);
        std::string key;
        long long value;
        std::string unit;
              line_stream >> key >> value >> unit;

        if (key == "MemTotal:") {
            mem.memTotal = value;
        } else if (key == "MemAvailable:") {
            mem.memAvailable = value;
        } else if (key == "SwapTotal:") {
            mem.swapTotal = value;
        } else if (key == "SwapFree:") {
            mem.swapFree = value;
        }
    }
    

    return mem;
}

std::vector<long long> get_cpu_times(const std::string& content) {
    std::stringstream ss(content);
    std::string line;
    std::getline(ss,line);

    std::stringstream line_ss(line);
    std::string label;
    line_ss >> label;

    std::vector<long long> times;
    long long time_value;

    while(line_ss >> time_value) {
        times.push_back(time_value);
    }
    return times;
}


CpuStats TelemetryCollector::parse_cpu() {
    CpuStats cpu;

    std::string content_t1 = readFile("proc/stat");
    std::vector<long long> t1 = get_cpu_times(content_t1);

    if(t1.empty() || t1.size()< 5) return  cpu;


    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::string content_t2 = readFile("/proc/stat");
    std::vector<long long > t2 = get_cpu_times(content_t2);

    if(t2.empty() || t2.size()< 5) return  cpu;

    long long idle_t1 = t1[3];
    long long  idle_t2 = t2[4];
    long long iowait_t1 = t1[4];
    long long iowait_t2 = t2[4];

    long long total_t1 = 0;
    long long total_t2 = 0;

    for(size_t i =0;i<t1.size();i++) {
        total_t1 += t1[i];
        total_t2 += t2[i];
    }

    long long total_diff = total_t2 - total_t1;
    long long idle_diff = idle_t2 - idle_t1;
    long long iowait_diff = iowait_t2 - iowait_t1;

    if (total_diff > 0) {
        cpu.usagePercentage = 100.0 * (total_diff - idle_diff) / total_diff;
        
        cpu.iowaitPercentage = 100.0 * iowait_diff / total_diff;
    }

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