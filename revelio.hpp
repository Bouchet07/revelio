#ifndef REVELIO_HPP
#define REVELIO_HPP

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <array>
#include <cstdint>
#include <type_traits>
#include <cctype>
#include <iterator>
#include <algorithm>

/**
 * @brief Revelio: A high-performance Sudoku solver.
 * Uses bitmasking, backtracking, and the Minimum Remaining Values (MRV) heuristic.
 */
class Revelio {
public:
    using Board = std::array<int, 81>;

    Revelio() {
        // Precompute box indices to avoid division in the hot loop
        for (int i = 0; i < 81; ++i) {
            int r = i / 9;
            int c = i % 9;
            box_map_[i] = (r / 3) * 3 + (c / 3);
        }
        reset();
    }

    void reset() {
        board_.fill(0);
        rows_.fill(0);
        cols_.fill(0);
        boxes_.fill(0);
    }

    /**
     * @brief Load from iterators. Supports char ('1'-'9', '0', '.') or int.
     */
    template <typename It>
    bool load(It begin, It end) {
        reset();
        int i = 0;
        for (auto it = begin; it != end && i < 81; ++it, ++i) {
            int num;
            using val_type = std::decay_t<typename std::iterator_traits<It>::value_type>;
            if constexpr (std::is_same_v<val_type, char>) {
                if (std::isdigit(static_cast<unsigned char>(*it))) num = *it - '0';
                else if (*it == '.' || *it == '0') num = 0;
                else { --i; continue; } // ignore formatting
            } else {
                num = static_cast<int>(*it);
            }
            
            if (num < 0 || num > 9) return false;
            if (num != 0) {
                if (!set(i, num)) return false; // Invalid puzzle
            }
        }
        return i == 81;
    }

    template <typename Iterable>
    bool load(const Iterable& input) {
        return load(std::begin(input), std::end(input));
    }

    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return false;
        std::vector<char> values;
        char ch;
        while (file >> ch && values.size() < 81) {
            if (std::isdigit(static_cast<unsigned char>(ch)) || ch == '.') {
                values.push_back(ch);
            }
        }
        return load(values);
    }

    /**
     * @brief Solve the puzzle using MRV backtracking.
     */
    bool revelio() {
        return backtrack();
    }

    void display() const {
        for (int r = 0; r < 9; ++r) {
            if (r % 3 == 0 && r != 0) std::cout << "------+-------+------\n";
            for (int c = 0; c < 9; ++c) {
                if (c % 3 == 0 && c != 0) std::cout << "| ";
                std::cout << board_[r * 9 + c] << " ";
            }
            std::cout << "\n";
        }
    }

    const Board& getBoard() const { return board_; }

private:
    Board board_;
    std::array<uint16_t, 9> rows_;
    std::array<uint16_t, 9> cols_;
    std::array<uint16_t, 9> boxes_;
    std::array<int, 81> box_map_;

    inline bool set(int idx, int num) {
        int r = idx / 9;
        int c = idx % 9;
        int box = box_map_[idx];
        uint16_t bit = 1 << (num - 1);
        if ((rows_[r] & bit) || (cols_[c] & bit) || (boxes_[box] & bit)) return false;
        board_[idx] = num;
        rows_[r] |= bit;
        cols_[c] |= bit;
        boxes_[box] |= bit;
        return true;
    }

    inline void unset(int idx, int num) {
        int r = idx / 9;
        int c = idx % 9;
        int box = box_map_[idx];
        uint16_t bit = 1 << (num - 1);
        board_[idx] = 0;
        rows_[r] &= ~bit;
        cols_[c] &= ~bit;
        boxes_[box] &= ~bit;
    }

    /**
     * @brief Backtrack using the Minimum Remaining Values (MRV) heuristic.
     * Selects the cell with the fewest possible legal moves first.
     */
    bool backtrack() {
        int best_idx = -1;
        int min_choices = 10;
        uint16_t best_mask = 0;

        // Find the cell with the minimum number of possibilities (MRV)
        for (int i = 0; i < 81; ++i) {
            if (board_[i] == 0) {
                int r = i / 9;
                int c = i % 9;
                int box = box_map_[i];
                uint16_t used = rows_[r] | cols_[c] | boxes_[box];
                uint16_t possible = (~used) & 0x1FF;
                
                int count = __builtin_popcount(possible);
                if (count == 0) return false; // Dead end
                
                if (count < min_choices) {
                    min_choices = count;
                    best_idx = i;
                    best_mask = possible;
                    if (count == 1) break; // Optimization: early exit if only one choice
                }
            }
        }

        if (best_idx == -1) return true; // Solved

        // Try numbers for the best cell
        while (best_mask) {
            int num_idx = __builtin_ctz(best_mask);
            int num = num_idx + 1;
            best_mask &= ~(1 << num_idx); // Clear the bit we're trying

            if (set(best_idx, num)) {
                if (backtrack()) return true;
                unset(best_idx, num);
            }
        }

        return false;
    }
};

#endif // REVELIO_HPP
