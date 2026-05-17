# Revelio Sudoku Solver

Revelio is an "incredible" high-performance, header-only C++ Sudoku solver. It achieves sub-50 microsecond solving times by combining bitmasking with advanced search heuristics and CPU intrinsics.

## Features

- **Microsecond Performance:** Solves most puzzles in ~10-30 microseconds.
- **MRV Heuristic:** Implements the **Minimum Remaining Values** heuristic to aggressively prune the search space.
- **Bit Manipulation Intrinsics:** Uses `__builtin_popcount` and `__builtin_ctz` for constant-time candidate analysis.
- **Header-Only:** Simply drop `revelio.hpp` into your project. No building required.
- **Flexible API:** 
  - Load from `std::string`, `std::vector`, or any C++ iterable.
  - Efficient file loading with lenient formatting (ignores `|`, `-`, etc.).
- **Zero Overhead:** Pre-computed box mappings and inlined hot paths.

## Installation

Since Revelio is a header-only library, you just need to include it:

```cpp
#include "revelio.hpp"
```

To build the included CLI tool:

```bash
g++ -O3 -o revelio revelio.cpp
```

## Usage

### Command Line

The CLI tool includes a built-in benchmarker:

```bash
# Solve a puzzle from a file
./revelio sample.txt

# Run the default example benchmark
./revelio
```

### Library Integration

```cpp
#include "revelio.hpp"

Revelio solver;
// Can be a string, vector<int>, or any iterable
std::string puzzle = "53..7....6..195....98....6.8...6...34..8.3..17...2...6.6....28....419..5....8..79";

if (solver.load(puzzle)) {
    if (solver.revelio()) {
        solver.display();
        // Access the solved board
        const auto& board = solver.getBoard();
    }
}
```

## How it Works

Revelio doesn't just "backtrack." It uses:
1. **Bitmasks:** Rows, columns, and 3x3 boxes are tracked in `uint16_t` bitsets.
2. **Fail-Fast Heuristics:** It always chooses the cell with the fewest remaining legal moves first (MRV).
3. **Hardware Acceleration:** Candidate selection is performed via trailing-zero counting at the CPU level.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
