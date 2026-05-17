#include "revelio.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <numeric>
#include <iomanip>
#include <fstream>

int main(int argc, char* argv[]) {
    std::string filename = (argc > 1) ? argv[1] : "top95.txt";
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open " << filename << "\n";
        return 1;
    }

    std::vector<std::string> puzzles;
    std::string line;
    while (std::getline(file, line)) {
        if (line.length() >= 81) puzzles.push_back(line);
    }

    Revelio solver;
    std::vector<double> times;
    int solved_count = 0;

    std::cout << "========================================\n";
    std::cout << "   Revelio Performance Benchmark       \n";
    std::cout << "   Dataset: " << filename << " (" << puzzles.size() << " puzzles)\n";
    std::cout << "========================================\n\n";

    auto total_start = std::chrono::high_resolution_clock::now();

    for (const auto& p : puzzles) {
        if (!solver.load(p)) continue;

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

    if (times.empty()) {
        std::cout << "No puzzles solved.\n";
        return 0;
    }

    double sum = std::accumulate(times.begin(), times.end(), 0.0);
    double avg = sum / times.size();
    double min_time = *std::min_element(times.begin(), times.end());
    double max_time = *std::max_element(times.begin(), times.end());

    std::cout << "Results:\n";
    std::cout << "  Puzzles Solved: " << solved_count << "/" << puzzles.size() << "\n";
    std::cout << "  Total Time:     " << std::fixed << std::setprecision(3) << total_elapsed.count() << " ms\n";
    std::cout << "  Average Time:   " << avg << " us\n";
    std::cout << "  Min Time:       " << min_time << " us\n";
    std::cout << "  Max Time:       " << max_time << " us\n";
    std::cout << "  Throughput:     " << (solved_count / (total_elapsed.count() / 1000.0)) << " puzzles/sec\n";
    std::cout << "\n========================================\n";

    return 0;
}
