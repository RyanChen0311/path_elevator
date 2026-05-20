# Elevator Simulator

> A console-based elevator dispatch simulator written in C++, demonstrating **object-oriented design**, **separation of concerns**, and a **nearest-car dispatch algorithm**.

---

## Overview

This project simulates a multi-elevator system in a 10-floor building.  When a rider requests a lift, the **Dispatcher** selects whichever elevator is currently closest (minimum floor distance) and routes it to pick up the rider, then deliver them to their destination.

The codebase is structured with three independent classes — `Elevator`, `Dispatcher`, and `UI` — each with a single, clearly defined responsibility.

---

## Architecture

```
┌─────────────────────────────────────────┐
│                  main.cpp               │
│  Creates elevators, dispatcher, and UI  │
└────────────┬────────────────────────────┘
             │
             ▼
┌────────────────────┐
│        UI          │  Input validation, event loop
└────────┬───────────┘
         │ rider_floor, dest_floor
         ▼
┌────────────────────┐
│     Dispatcher     │  Nearest-car selection policy
└────────┬───────────┘
         │ move(rider_floor, dest_floor)
         ▼
┌────────────────────┐
│     Elevator       │  Travel simulation, floor printing
└────────────────────┘
```

### Class responsibilities

| Class | Responsibility |
|---|---|
| `Elevator` | Tracks current floor; simulates travel by printing each floor passed |
| `Dispatcher` | Selects the nearest elevator using `min(|current − rider|)` |
| `UI` | Handles all console I/O, input validation, and the main loop |

---

## Dispatch Algorithm

The **nearest-car policy** minimises average passenger wait time in small buildings:

```
For each elevator e:
    distance[e] = |e.currentFloor − rider_floor|

Dispatch elevator with minimum distance
```

This is one of the standard algorithms used in real low-rise elevator controllers, alongside more complex strategies like SCAN (the "elevator algorithm") and LOOK.

---

## Project Structure

```
Elevator-Simulator/
├── src/
│   ├── main.cpp          # Entry point — wires components together
│   ├── Elevator.cpp      # Elevator movement and display
│   ├── Dispatcher.cpp    # Nearest-car dispatch logic
│   └── UI.cpp            # Console input loop and validation
├── include/
│   ├── Elevator.h
│   ├── Dispatcher.h
│   └── UI.h
└── Makefile
```

---

## Build & Run

**Requirements:** g++ ≥ 9 (C++17), GNU Make

```bash
# Build
make

# Build and run immediately
make run

# Debug build (AddressSanitizer enabled)
make debug

# Clean
make clean
```

---

## Sample Session

```
=== Elevator Simulator ===
Building: floors 1 – 10
Enter 0 at any prompt to quit.

Press Enter to call an elevator (or 0 to quit): 1
Your current floor: 3
Your desired floor:  8

Dispatching elevator A (nearest to floor 3)

Elevator A dispatched (currently at floor 1)
--- Travelling to pick up rider at floor 3 ---
  [A] floor 1
  [A] floor 2
  [A] floor 3
--- Rider boarded. Travelling to floor 8 ---
  [A] floor 3
  [A] floor 4
  [A] floor 5
  [A] floor 6
  [A] floor 7
  [A] floor 8
============================
Elevator A arrived at floor 8
============================
```

---

## Design Notes

- **No global state** — all state is encapsulated in class instances.
- **Input validation** — the UI rejects out-of-range floors and non-numeric input without crashing.
- **Extensible** — adding a third elevator requires only one line in `main.cpp`; the dispatcher handles any number of elevators automatically.
- **Separation of concerns** — swapping the dispatch policy (e.g. to SCAN) only requires changing `Dispatcher.cpp`.

---

## License

MIT — see [LICENSE](LICENSE).
