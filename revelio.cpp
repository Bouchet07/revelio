#include "revelio.hpp"
#include <iostream>
#include <string>
#include <chrono>

int main(int argc, char* argv[]) {
    Revelio solver;
    std::string path;

    if (argc > 1) {
        path = argv[1];
    } else {
        std::cout << "Usage: " << argv[0] << " <sudoku_file>\n";
        std::cout << "Running default example...\n";
    }

    bool success = path.empty() 
        ? solver.load("53..7....6..195....98....6.8...6...34..8.3..17...2...6.6....28....419..5....8..79")
        : solver.loadFromFile(path);

    if (success) {
        std::cout << "Initial board:\n";
        solver.display();

        auto start = std::chrono::high_resolution_clock::now();
        bool solved = solver.revelio();
        auto end = std::chrono::high_resolution_clock::now();
        
        std::chrono::duration<double, std::micro> elapsed = end - start;

        if (solved) {
            std::cout << "\nSolved board (found in " << elapsed.count() << " us):\n";
            solver.display();
        } else {
            std::cout << "\nNo solution found.\n";
        }
    } else if (!path.empty()) {
        std::cerr << "Failed to load puzzle from " << path << "\n";
        return 1;
    }

    return 0;
}
