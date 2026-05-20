/**
 * @file Elevator.h
 * @brief Elevator class — models a single elevator car
 *
 * Tracks the current floor of one elevator and handles movement,
 * printing each floor as it passes (simulating real-time travel).
 *
 * @author Ryan Chen
 * @date 2024
 */

#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <string>

/**
 * @brief Models a single elevator car in the building.
 *
 * Each Elevator has a name and a current floor.  It can be commanded
 * to move to any floor; it prints every intermediate floor as it
 * travels, giving a visual sense of motion.
 */
class Elevator {
public:
    /**
     * @brief Construct an elevator with a given name and optional starting floor.
     * @param name  Display name (e.g. "A", "B").
     * @param floor Initial floor (default 1).
     */
    explicit Elevator(const std::string &name, int floor = 1);

    /**
     * @brief Move this elevator to @p dest_floor, picking up a rider at @p rider_floor first.
     *
     * Prints every floor passed during both legs of the journey:
     *   1. Current floor → rider's floor (pickup)
     *   2. Rider's floor → destination floor (delivery)
     *
     * @param rider_floor  Floor where the rider is waiting.
     * @param dest_floor   Floor the rider wants to reach.
     */
    void move(int rider_floor, int dest_floor);

    /**
     * @brief Print the elevator's current position.
     */
    void display() const;

    /**
     * @brief Return the elevator's current floor.
     */
    int currentFloor() const { return current_floor_; }

    /**
     * @brief Return the elevator's name.
     */
    const std::string &name() const { return name_; }

private:
    std::string name_;
    int         current_floor_;

    /**
     * @brief Print every floor from @p from to @p to (inclusive), ascending or descending.
     */
    void printTravel(int from, int to) const;
};

#endif /* ELEVATOR_H */
