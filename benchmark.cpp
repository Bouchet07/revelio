#include "revelio.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <numeric>
#include <iomanip>

/**
 * Benchmark suite for Revelio.
 * Includes puzzles from the "top95" dataset (extremely difficult puzzles).
 */
int main() {
    std::vector<std::string> top95 = {
        "4.....8.5.3..........7......2.....6.....8.4......1.......6.3.7.5..2.....1.4......",
        "52...6.........7.13...........4..8..6......5...........418.........3..2...87.....",
        "6.....8.3.4.7.................5.4.7.3..2.....1.6.......2.....5.....8.6......1....",
        "48.3............71.2.......7.5....6....2..8.............1.76...3.....4......5....",
        "....14....3....2...7..........9...3.6.1.............8.2.....1.4....5.6.....7.8...",
        ".......39.....1..5..1.24..7.9...7.8....5.9.4......8..7..8...2.6.3.4.5.7.2.......",
        "3..4..1...8.1..2...7...2..5.6..1...2..3...7..8...3..4.5..2...8...7..5.3...2..9..4",
        ".......8..3..4...1..1..2..6.6..1...2..3...7..8...3..4.5..2...8...7..5.3...2..9..4",
        ".......8.9...3.67..1.2.......8....4.6...7...1.5....2.......1.3..45.3...2.8.......",
        "....7..2.8.......6.1.2.5...9.54....8.........3....85.1...3.2.8.4.......9.7..6...."
    };

    Revelio solver;
    std::vector<double> times;
    int solved_count = 0;

    std::cout << "========================================\n";
    std::cout << "   Revelio Performance Benchmark       \n";
    std::cout << "   (Selected Top95 Hard Puzzles)       \n";
    std::cout << "========================================\n\n";

    auto total_start = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < top95.size(); ++i) {
        if (!solver.load(top95[i])) continue;

        auto start = std::chrono::high_resolution_clock::now();
        bool solved = solver.revelio();
        auto end = std::chrono::high_resolution_clock::now();

        if (solved) {
            std::chrono::duration<double, std::micro> elapsed = end - start;
            times.push_back(elapsed.count());
            solved_count++;
        }
    }

    auto total_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> total_elapsed = total_end - total_start;

    double sum = std::accumulate(times.begin(), times.end(), 0.0);
    double avg = sum / times.size();
    double min_time = *std::min_element(times.begin(), times.end());
    double max_time = *std::max_element(times.begin(), times.end());

    std::cout << "Results:\n";
    std::cout << "  Puzzles Solved: " << solved_count << "/" << top95.size() << "\n";
    std::cout << "  Total Time:     " << std::fixed << std::setprecision(3) << total_elapsed.count() << " ms\n";
    std::cout << "  Average Time:   " << avg << " us\n";
    std::cout << "  Min Time:       " << min_time << " us\n";
    std::cout << "  Max Time:       " << max_time << " us\n";
    std::cout << "  Throughput:     " << (solved_count / (total_elapsed.count() / 1000.0)) << " puzzles/sec\n";
    std::cout << "\n========================================\n";

    return 0;
}
