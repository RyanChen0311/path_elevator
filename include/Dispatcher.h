/**
 * @file Dispatcher.h
 * @brief Dispatcher class — selects the closest elevator for a request
 *
 * The Dispatcher holds a list of elevators and applies a nearest-car
 * policy: whichever elevator is closest (by floor distance) to the
 * rider is dispatched.  Ties are broken in favour of the lower-index
 * elevator.
 *
 * @author Ryan Chen
 * @date 2024
 */

#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "Elevator.h"
#include <vector>

/**
 * @brief Dispatches the nearest elevator to a rider's request.
 *
 * Design note: the nearest-car heuristic minimises average wait time
 * for small buildings and is the algorithm used in many real low-rise
 * elevator controllers.
 */
class Dispatcher {
public:
    /**
     * @brief Construct a dispatcher managing the given elevators.
     * @param elevators  List of elevators in the building (at least one).
     */
    explicit Dispatcher(std::vector<Elevator> &elevators);

    /**
     * @brief Dispatch the nearest elevator to serve a ride request.
     *
     * Selects the elevator with the minimum |current_floor - rider_floor|
     * and commands it to travel to @p rider_floor then @p dest_floor.
     *
     * @param rider_floor  Floor where the rider is waiting.
     * @param dest_floor   Floor the rider wants to reach.
     */
    void dispatch(int rider_floor, int dest_floor);

private:
    std::vector<Elevator> &elevators_;

    /**
     * @brief Return the index of the elevator closest to @p floor.
     */
    int nearestElevatorIndex(int floor) const;
};

#endif /* DISPATCHER_H */
