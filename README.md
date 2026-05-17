# Revelio Sudoku Solver

Revelio is a fast, clean, and performant C++ Sudoku solver. It utilizes a bitmask-optimized backtracking algorithm to achieve high execution speeds while maintaining a simple and idiomatic C++ API.

## Features

- **Blazing Fast:** Uses bitmasks (`uint16_t`) for $O(1)$ constraint checking.
- **Flexible API:** 
  - Load puzzles from `std::string`, `std::vector`, or any C++ iterable.
  - Load puzzles directly from files.
  - Supports multiple input formats: digits (`1`-`9`), zeros (`0`), or dots (`.`).
- **Clean Code:** Encapsulated in a single, easy-to-use `Revelio` class.
- **CLI Tool:** Includes a built-in command-line interface for quick solving.

## Installation

Simply compile the `revelio.cpp` file using a modern C++ compiler (C++17 or later recommended):

```bash
g++ -O3 -o revelio revelio.cpp
```

## Usage

### Command Line

You can run the solver by passing a file path or letting it run the default example:

```bash
# Solve a puzzle from a file
./revelio sample.txt

# Run the default example
./revelio
```

### As a Library

Include the `Revelio` class in your project to use it programmatically:

```cpp
#include "revelio.cpp" // Or move the class to a header

Revelio solver;
std::string puzzle = "53..7....6..195....98....6.8...6...34..8.3..17...2...6.6....28....419..5....8..79";

if (solver.load(puzzle)) {
    if (solver.revelio()) {
        solver.display();
    } else {
        std::cout << "No solution found." << std::endl;
    }
}
```

## Input Format

The solver is lenient with input. It looks for 81 digits or dots. Spaces, pipes (`|`), and dashes (`-`) are ignored, making it easy to read "pretty-printed" Sudokus:

```text
5 3 . | . 7 . | . . .
6 . . | 1 9 5 | . . .
. 9 8 | . . . | . 6 .
------+-------+------
...
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
