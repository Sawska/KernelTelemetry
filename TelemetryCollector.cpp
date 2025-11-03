#include "TelemetryCollector.h"
#include "Metrics.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <chrono>

struct DiskStatsRaw {
    long long readsCompleted = 0;
    long long sectorsRead = 0;
    long long writesCompleted = 0;
    long long sectorsWritten = 0;
    long long ioTimesMs = 0;
};

struct NetworkStatsRaw {
    long long rxBytes = 0;
    long long txBytes = 0;
    long long rxPackets = 0;
    long long txPackets = 0;
};


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

    std::string content_t1 = readFile("/proc/stat");
    std::vector<long long> t1 = get_cpu_times(content_t1);

    if (t1.empty() || t1.size() < 5) return cpu;

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::string content_t2 = readFile("/proc/stat");
    std::vector<long long> t2 = get_cpu_times(content_t2);

    if (t2.empty() || t2.size() < 5) return cpu;

    long long idle_t1 = t1[3];
    long long idle_t2 = t2[3];
    long long iowait_t1 = t1[4];
    long long iowait_t2 = t2[4];

    long long total_t1 = 0;
    long long total_t2 = 0;

    for (size_t i = 0; i < t1.size(); i++) {
        total_t1 += t1[i];
        total_t2 += t2[i];
    }

    long long total_diff = total_t2 - total_t1;
    long long idle_diff = idle_t2 - idle_t1;
    long long iowait_diff = iowait_t2 - iowait_t1;

    if (total_diff > 0) {
        
        long long used_diff = total_diff - idle_diff;

        cpu.usagePercentage = 100.0 * (static_cast<double>(used_diff)) / static_cast<double>(total_diff);
        
        cpu.iowaitPercentage = 100.0 * (static_cast<double>(iowait_diff)) / static_cast<double>(total_diff);
    } else {
        cpu.usagePercentage = 0.0;
        cpu.iowaitPercentage = 0.0;
    }

    return cpu;
}


std::map<std::string,DiskStatsRaw> get_disk_raw_stats() {
    std::map<std::string,DiskStatsRaw> raw_stats;
    std::string content = readFile("/proc/diskstats");
    if (content.empty()) return raw_stats;

    std::stringstream ss(content);
    std::string line;

    while (std::getline(ss,line)) {
        std::stringstream line_ss(line);
        int major,minor;
        std::string deviceName;
        DiskStatsRaw raw;


        if (!(line_ss >> major >> minor >> deviceName)) continue;

        if (std::isdigit(deviceName.back()) && deviceName.rfind("nvme", 0) != 0)
            continue;


        long long dummy,readMerged,writeMerged,dummy2,ioProgress,dummy3;

        if (!(line_ss >> raw.readsCompleted)) continue;
        
        if (!(line_ss >> readMerged)) continue;
        
        if (!(line_ss >> raw.sectorsRead)) continue;
        
        if (!(line_ss >> dummy)) continue;
        
        if (!(line_ss >> raw.writesCompleted)) continue;
        
        if (!(line_ss >> writeMerged)) continue;
        
        if (!(line_ss >> raw.sectorsWritten)) continue;
        
        if (!(line_ss >> dummy)) continue;
        
        if (!(line_ss >> ioProgress)) continue;
        
        if (!(line_ss >> raw.ioTimesMs)) continue;

        raw_stats[deviceName] = raw;
    }

    return raw_stats;
    
}

std::vector<DiskStats> TelemetryCollector::parse_diskstats() {
    std::vector<DiskStats> allDisks;

    std::map<std::string,DiskStatsRaw> t1 = get_disk_raw_stats();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::map<std::string,DiskStatsRaw> t2 = get_disk_raw_stats();

    for(const auto& pair :t2) {
        const std::string& deviceName = pair.first;
        const DiskStatsRaw& raw_t2 = pair.second;

        if (t1.count(deviceName)) {
            const DiskStatsRaw& raw_t1 = t1.at(deviceName);

            long long reads_diff = raw_t2.readsCompleted - raw_t1.readsCompleted;
            long long writes_diff = raw_t2.writesCompleted - raw_t1.writesCompleted;
            long long sectors_read_diff = raw_t2.sectorsRead - raw_t1.sectorsRead;
            long long sectors_written_diff = raw_t2.sectorsWritten - raw_t1.sectorsWritten;

            DiskStats disk;

            disk.deviceName = deviceName;

            disk.readsPerSecond = static_cast<double>(reads_diff);
            disk.writesPerSecond = static_cast<double>(writes_diff);

            disk.megabytesReadPerSecond = sectors_read_diff * SECTOR_SIZE_BYTES / 1024.0 / 1024.0;
            disk.megabytesWrittenPerSecond = sectors_written_diff * SECTOR_SIZE_BYTES / 1024.0 / 1024.0;

            allDisks.push_back(disk);
        }
    }

    return allDisks;
}

std::map<std::string,NetworkStatsRaw> get_net_raw_stats() {
    std::map<std::string,NetworkStatsRaw> raw_stats;
    std::string content = readFile("/proc/net/dev");
    if(content.empty()) return raw_stats;

    std::stringstream ss(content);
    std::string line;

    std::getline(ss,line);
    std::getline(ss,line);

    while(std::getline(ss,line)) {
        line.erase(0,line.find_first_not_of("\t"));
        if (line.empty()) continue;

    std::stringstream line_ss(line);
    std::string interfaceName;
    NetworkStatsRaw raw;

    if (!(line_ss >> interfaceName)) continue;
    if (interfaceName.back() == ';') {
        interfaceName.pop_back();
    } else {
        continue;
    }

    if (interfaceName == "io") continue;

    long long dummy;

    if (!(line_ss >> raw.rxBytes >> raw.rxPackets >> dummy >> dummy)) continue;

    if (!(line_ss >> raw.txBytes >> raw.txPackets)) continue;

    raw_stats[interfaceName] = raw;
    }
    return raw_stats;
}


std::vector<NetworkStats> TelemetryCollector::parse_net_dev() {
    std::vector<NetworkStats> allInterfaces;

    std::map<std::string,NetworkStatsRaw> t1 = get_net_raw_stats();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::map<std::string,NetworkStatsRaw> t2 = get_net_raw_stats();

    for(const auto& pair : t2) {
        const std::string& interfaceName = pair.first;
        const NetworkStatsRaw& raw_t2 = pair.second;

        if(t1.count(interfaceName)) {
            const NetworkStatsRaw raw_t1 = t1.at(interfaceName);

            long long rx_bytes_diff = raw_t2.rxBytes - raw_t1.rxBytes;
            long long tx_bytes_diff = raw_t2.txBytes - raw_t1.txBytes;
            long long rx_packets_diff = raw_t2.rxPackets - raw_t1.rxPackets;
            long long tx_packets_diff = raw_t2.txPackets - raw_t1.txPackets;

            NetworkStats iface;

            iface.interfaceName = interfaceName;

            iface.megabytesReceivedPerSecond = rx_bytes_diff / 1024.0 / 1024.0;
            iface.megabytesSentPerSecond = tx_bytes_diff / 1024.0 / 1024.0;


              iface.packetsReceivedPerSecond = static_cast<double>(rx_packets_diff);
            iface.packetsSentPerSecond = static_cast<double>(tx_packets_diff);

            allInterfaces.push_back(iface);
        }

    }

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