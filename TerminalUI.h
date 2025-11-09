#ifndef TERMINAL_UI_H
#define TERMINAL_UI_H

#include "MonitorManager.h"
#include <ncurses.h>
#include <string>
#include <vector>

class TerminalUI {
public:
    TerminalUI(MonitorManager &monitor);
    ~TerminalUI();

    void configure(const std::string &sort_type, int interval);
    void start();

private:
    void draw(const std::vector<ProcessInfo> &procs, double cpu, uint64_t mem_used, uint64_t mem_total);
    void handleInput();
    void sortProcesses(std::vector<ProcessInfo> &procs);

    MonitorManager &monitor;
    std::string sort_mode = "cpu";
    int refresh_interval = 2;
    bool running = true;
};

#endif
