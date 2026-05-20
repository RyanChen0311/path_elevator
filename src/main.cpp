/**
 * @file main.cpp
 * @brief Elevator Simulator — entry point
 *
 * Wires together two elevators, a dispatcher, and the console UI.
 * The number of elevators and their starting floors can be adjusted here.
 *
 * @author Ryan Chen
 * @date 2024
 */

#include "Elevator.h"
#include "Dispatcher.h"
#include "UI.h"
#include <vector>

int main()
{
    /* --- Configure elevators --- */
    std::vector<Elevator> elevators = {
        Elevator("A", 1),
        Elevator("B", 5),
    };

    /* --- Wire up dispatcher and UI --- */
    Dispatcher dispatcher(elevators);
    UI         ui(dispatcher);

    ui.run();
    return 0;
}
