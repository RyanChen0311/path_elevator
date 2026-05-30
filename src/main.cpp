/**
 * @file main.cpp
 * @brief ud — path hierarchy traversal tool
 *
 * Usage:
 *   ./ud <start> <end> [<stop>]
 *
 *   start / end  directory names from the CWD ancestor chain;
 *                their relative depths determine direction:
 *                  start shallower than end  -> DESCEND (root -> target)
 *                  start deeper    than end  -> ASCEND  (target -> root)
 *
 *   stop         optional display limit (must lie between start and end)
 *
 * @author Ryan Chen
 * @date 2024
 */

#include "PathElevator.h"
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char *argv[])
{
    if (argc >= 2) {
        std::vector<std::string> args(argv + 1, argv + argc);
        PathElevator::ud(args);
        return 0;
    }

    std::cout << "Usage: ud <start> <end> [<stop>]\n"
              << "\n"
              << "  start  first directory name to display\n"
              << "  end    determines direction (shallower->deeper = DESCEND, deeper->shallower = ASCEND)\n"
              << "         the full [start,end] range is pre-cached\n"
              << "  stop   optional: stop display here (between start and end)\n"
              << "\n"
              << "Both names must appear in the current directory's ancestor chain.\n";
    return 1;
}
