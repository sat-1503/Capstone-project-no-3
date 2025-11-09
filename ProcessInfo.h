#ifndef PROCESS_INFO_H
#define PROCESS_INFO_H

#include <string>
#include <cstdint>

struct ProcessInfo {
    int pid = 0;
    std::string user;
    std::string name;
    uint64_t mem_kb = 0;
    double cpu_usage = 0.0;
    long threads = 0;
};

#endif
