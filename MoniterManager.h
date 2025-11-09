#ifndef MONITOR_MANAGER_H
#define MONITOR_MANAGER_H

#include "ProcessInfo.h"
#include <vector>

class MonitorManager {
public:
    MonitorManager();
    void refresh();
    std::vector<ProcessInfo> getProcesses() const;

    double getCPUPercent();
    uint64_t getTotalMemKB();
    uint64_t getUsedMemKB();

private:
    std::vector<ProcessInfo> processes;
    uint64_t prev_total = 0;
    uint64_t prev_idle = 0;
};

#endif
