/**
 * @file Elevator.cpp
 * @brief Elevator class implementation
 */

#include "Elevator.h"
#include <iostream>

Elevator::Elevator(const std::string &name, int floor)
    : name_(name), current_floor_(floor)
{}

void Elevator::printTravel(int from, int to) const
{
    if (from <= to) {
        for (int f = from; f <= to; ++f)
            std::cout << "  [" << name_ << "] floor " << f << "\n";
    } else {
        for (int f = from; f >= to; --f)
            std::cout << "  [" << name_ << "] floor " << f << "\n";
    }
}

void Elevator::move(int rider_floor, int dest_floor)
{
    std::cout << "\nElevator " << name_
              << " dispatched (currently at floor " << current_floor_ << ")\n";
    std::cout << "--- Travelling to pick up rider at floor "
              << rider_floor << " ---\n";
    printTravel(current_floor_, rider_floor);

    current_floor_ = rider_floor;

    std::cout << "--- Rider boarded. Travelling to floor "
              << dest_floor << " ---\n";
    printTravel(current_floor_, dest_floor);

    current_floor_ = dest_floor;
    display();
}

void Elevator::display() const
{
    std::cout << "============================\n"
              << "Elevator " << name_
              << " arrived at floor " << current_floor_ << "\n"
              << "============================\n";
}
