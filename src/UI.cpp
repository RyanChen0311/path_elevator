/**
 * @file UI.cpp
 * @brief UI class implementation — console input loop with validation
 */

#include "UI.h"
#include <iostream>
#include <limits>

UI::UI(Dispatcher &dispatcher)
    : dispatcher_(dispatcher)
{}

int UI::readFloor(const std::string &prompt) const
{
    int floor;
    while (true) {
        std::cout << prompt;
        if (std::cin >> floor) {
            if (floor >= FLOOR_MIN && floor <= FLOOR_MAX)
                return floor;
            std::cout << "  Invalid floor. Please enter a number between "
                      << FLOOR_MIN << " and " << FLOOR_MAX << ".\n";
        } else {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "  Invalid input. Please enter a number.\n";
        }
    }
}

void UI::run()
{
    std::cout << "=== Elevator Simulator ===\n"
              << "Building: floors " << FLOOR_MIN << " – " << FLOOR_MAX << "\n"
              << "Enter 0 at any prompt to quit.\n\n";

    while (true) {
        std::cout << "Press Enter to call an elevator (or 0 to quit): ";
        int choice;
        std::cin >> choice;
        if (choice == 0) break;

        int rider_floor = readFloor("Your current floor: ");
        int dest_floor  = readFloor("Your desired floor:  ");

        dispatcher_.dispatch(rider_floor, dest_floor);
        std::cout << "\n";
    }

    std::cout << "Goodbye.\n";
}
