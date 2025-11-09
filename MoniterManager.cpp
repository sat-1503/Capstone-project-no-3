#include "MonitorManager.h"
#include <libproc.h>
#include <sys/sysctl.h>
#include <mach/mach.h>
#include <mach/mach_host.h>
#include <unistd.h>
#include <pwd.h>
#include <vector>

MonitorManager::MonitorManager() {
    refresh();
}

// CPU usage
double MonitorManager::getCPUPercent() {
    host_cpu_load_info_data_t info;
    mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
    if (host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t)&info, &count) != KERN_SUCCESS)
        return 0.0;

    uint64_t total = info.cpu_ticks[CPU_STATE_USER] + info.cpu_ticks[CPU_STATE_SYSTEM] +
                     info.cpu_ticks[CPU_STATE_IDLE] + info.cpu_ticks[CPU_STATE_NICE];
    uint64_t idle = info.cpu_ticks[CPU_STATE_IDLE];

    double diff_total = total - prev_total;
    double diff_idle = idle - prev_idle;
    prev_total = total;
    prev_idle = idle;

    return diff_total == 0 ? 0.0 : 100.0 * (1.0 - diff_idle / diff_total);
}

// Total memory in KB
uint64_t MonitorManager::getTotalMemKB() {
    int64_t mem;
    size_t len = sizeof(mem);
    if (sysctlbyname("hw.memsize", &mem, &len, NULL, 0) == 0)
        return mem / 1024;
    return 0;
}

// Used memory in KB
uint64_t MonitorManager::getUsedMemKB() {
    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    vm_statistics64_data_t vmstat;
    if (host_statistics64(mach_host_self(), HOST_VM_INFO64, (host_info64_t)&vmstat, &count) != KERN_SUCCESS)
        return 0;
    uint64_t used = (vmstat.active_count + vmstat.wire_count + vmstat.inactive_count) * getpagesize();
    return used / 1024;
}

// Process list
void MonitorManager::refresh() {
    processes.clear();
    int bufsize = proc_listpids(PROC_ALL_PIDS, 0, NULL, 0);
    std::vector<pid_t> pids(bufsize / sizeof(pid_t));
    proc_listpids(PROC_ALL_PIDS, 0, &pids[0], bufsize);

    for (pid_t pid : pids) {
        if (pid <= 0) continue;
        struct proc_bsdinfo bsdinfo;
        if (proc_pidinfo(pid, PROC_PIDTBSDINFO, 0, &bsdinfo, sizeof(bsdinfo)) <= 0)
            continue;
        struct proc_taskinfo taskinfo;
        if (proc_pidinfo(pid, PROC_PIDTASKINFO, 0, &taskinfo, sizeof(taskinfo)) <= 0)
            continue;

        ProcessInfo p;
        p.pid = pid;
        p.name = bsdinfo.pbi_comm;
        struct passwd *pw = getpwuid(bsdinfo.pbi_uid);
        p.user = pw ? pw->pw_name : std::to_string(bsdinfo.pbi_uid);
        p.mem_kb = taskinfo.pti_resident_size / 1024;
        p.threads = taskinfo.pti_threadnum;
        processes.push_back(p);
    }
}

std::vector<ProcessInfo> MonitorManager::getProcesses() const {
    return processes;
}
