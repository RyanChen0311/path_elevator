/**
 * @file UI.h
 * @brief UI class — console input loop
 *
 * Handles all user interaction: prompts, input validation, and the
 * main event loop.  Keeps I/O completely separate from dispatch logic.
 *
 * @author Ryan Chen
 * @date 2024
 */

#ifndef UI_H
#define UI_H

#include "Dispatcher.h"

/** Lowest valid floor in the building. */
constexpr int FLOOR_MIN = 1;

/** Highest valid floor in the building. */
constexpr int FLOOR_MAX = 10;

/**
 * @brief Console UI for the elevator simulator.
 *
 * Runs an interactive loop: asks the user for their current floor and
 * desired floor, validates the input, and forwards each request to the
 * Dispatcher.  The loop exits when the user enters 0.
 */
class UI {
public:
    /**
     * @brief Construct the UI with a reference to the dispatcher.
     * @param dispatcher  The dispatcher that will handle each ride request.
     */
    explicit UI(Dispatcher &dispatcher);

    /**
     * @brief Start the main interactive loop.
     *
     * Repeatedly prompts until the user enters 0 to quit.
     */
    void run();

private:
    Dispatcher &dispatcher_;

    /**
     * @brief Prompt for and return a validated floor number.
     * @param prompt  Message shown to the user.
     * @return        A floor in [FLOOR_MIN, FLOOR_MAX].
     */
    int readFloor(const std::string &prompt) const;
};

#endif /* UI_H */
