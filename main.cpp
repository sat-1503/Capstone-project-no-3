#include "MonitorManager.h"
#include "TerminalUI.h"
#include <iostream>
#include <string>

int main(int argc, char **argv) {
    int interval = 2;
    std::string sort_by = "cpu";

    // Command-line argument parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-i" && i + 1 < argc) {
            interval = std::stoi(argv[++i]);
            if (interval < 1) interval = 1;
        } else if (arg == "-s" && i + 1 < argc) {
            sort_by = argv[++i];
        } else if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: " << argv[0] 
                      << " [-i seconds] [-s cpu|mem|pid]\n";
            return 0;
        }
    }

    try {
        MonitorManager monitor;
        TerminalUI ui(monitor);

        ui.configure(sort_by, interval);
        ui.start();
    } 
    catch (const std::exception &e) {
        std::cerr << "Fatal Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
