#include "TerminalUI.h"
#include <algorithm>
#include <thread>
#include <chrono>

TerminalUI::TerminalUI(MonitorManager &monitor_ref) : monitor(monitor_ref) {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
}

TerminalUI::~TerminalUI() {
    endwin();
}

void TerminalUI::configure(const std::string &sort_type, int interval) {
    sort_mode = sort_type;
    refresh_interval = interval;
}

void TerminalUI::sortProcesses(std::vector<ProcessInfo> &procs) {
    if (sort_mode == "mem")
        std::sort(procs.begin(), procs.end(), [](auto &a, auto &b) { return a.mem_kb > b.mem_kb; });
    else if (sort_mode == "pid")
        std::sort(procs.begin(), procs.end(), [](auto &a, auto &b) { return a.pid < b.pid; });
}

void TerminalUI::draw(const std::vector<ProcessInfo> &procs, double cpu, uint64_t mem_used, uint64_t mem_total) {
    clear();
    mvprintw(0, 0, "Alternate System Monitor (press 'q' to quit)");
    mvprintw(1, 0, "CPU: %.2f%% | MEM: %llu / %llu KB | Sort: %s | Refresh: %ds",
             cpu, (unsigned long long)mem_used, (unsigned long long)mem_total,
             sort_mode.c_str(), refresh_interval);

    mvprintw(3, 0, "%5s %8s %10s %10s %10s", "PID", "USER", "MEM(KB)", "THREADS", "NAME");

    int row = 4;
    for (const auto &p : procs) {
        if (row >= LINES - 2) break;
        mvprintw(row++, 0, "%5d %8s %10llu %10ld %10s",
                 p.pid, p.user.c_str(), (unsigned long long)p.mem_kb, p.threads, p.name.c_str());
    }
    refresh();
}

void TerminalUI::handleInput() {
    int ch = getch();
    if (ch == 'q' || ch == 'Q') running = false;
}

void TerminalUI::start() {
    while (running) {
        monitor.refresh();
        auto procs = monitor.getProcesses();
        sortProcesses(procs);

        double cpu = monitor.getCPUPercent();
        uint64_t mem_total = monitor.getTotalMemKB();
        uint64_t mem_used = monitor.getUsedMemKB();

        draw(procs, cpu, mem_used, mem_total);
        handleInput();

        std::this_thread::sleep_for(std::chrono::seconds(refresh_interval));
    }
}
