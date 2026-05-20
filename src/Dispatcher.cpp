/**
 * @file Dispatcher.cpp
 * @brief Dispatcher class implementation — nearest-car policy
 */

#include "Dispatcher.h"
#include <cstdlib>
#include <climits>
#include <iostream>

Dispatcher::Dispatcher(std::vector<Elevator> &elevators)
    : elevators_(elevators)
{}

int Dispatcher::nearestElevatorIndex(int floor) const
{
    int best_idx  = 0;
    int best_dist = INT_MAX;

    for (int i = 0; i < static_cast<int>(elevators_.size()); ++i) {
        int dist = std::abs(elevators_[i].currentFloor() - floor);
        if (dist < best_dist) {
            best_dist = dist;
            best_idx  = i;
        }
    }
    return best_idx;
}

void Dispatcher::dispatch(int rider_floor, int dest_floor)
{
    int idx = nearestElevatorIndex(rider_floor);
    std::cout << "\nDispatching elevator " << elevators_[idx].name()
              << " (nearest to floor " << rider_floor << ")\n";
    elevators_[idx].move(rider_floor, dest_floor);
}
