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
 * Uses bitmasking, backtracking, and optimized Minimum Remaining Values (MRV).
 */
class Revelio {
public:
    using Board = std::array<int, 81>;

    Revelio() {
        for (int i = 0; i < 81; ++i) {
            int r = i / 9;
            int c = i % 9;
            box_map_[i] = (r / 3) * 3 + (c / 3);
            row_map_[i] = r;
            col_map_[i] = c;
        }
        reset();
    }

    void reset() {
        board_.fill(0);
        rows_.fill(0);
        cols_.fill(0);
        boxes_.fill(0);
        empty_cells_.clear();
    }

    template <typename It>
    bool load(It begin, It end) {
        reset();
        int i = 0;
        std::vector<int> initial_values;
        for (auto it = begin; it != end && i < 81; ++it, ++i) {
            int num;
            using val_type = std::decay_t<typename std::iterator_traits<It>::value_type>;
            if constexpr (std::is_same_v<val_type, char>) {
                if (std::isdigit(static_cast<unsigned char>(*it))) num = *it - '0';
                else if (*it == '.' || *it == '0') num = 0;
                else { --i; continue; }
            } else {
                num = static_cast<int>(*it);
            }
            initial_values.push_back(num);
        }

        if (i != 81) return false;

        for (int idx = 0; idx < 81; ++idx) {
            if (initial_values[idx] != 0) {
                if (!set(idx, initial_values[idx])) return false;
            } else {
                empty_cells_.push_back(idx);
            }
        }
        return true;
    }

    template <typename Iterable>
    bool load(const Iterable& input) {
        return load(std::begin(input), std::end(input));
    }

    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return false;
        std::string content;
        char ch;
        while (file >> ch) content += ch;
        return load(content);
    }

    bool revelio() {
        return backtrack(0);
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
    std::array<int, 81> row_map_;
    std::array<int, 81> col_map_;
    std::vector<int> empty_cells_;

    inline bool set(int idx, int num) {
        uint16_t bit = 1 << (num - 1);
        int r = row_map_[idx];
        int c = col_map_[idx];
        int b = box_map_[idx];
        if ((rows_[r] & bit) || (cols_[c] & bit) || (boxes_[b] & bit)) return false;
        board_[idx] = num;
        rows_[r] |= bit;
        cols_[c] |= bit;
        boxes_[b] |= bit;
        return true;
    }

    inline void unset(int idx, int num) {
        uint16_t bit = 1 << (num - 1);
        board_[idx] = 0;
        rows_[row_map_[idx]] &= ~bit;
        cols_[col_map_[idx]] &= ~bit;
        boxes_[box_map_[idx]] &= ~bit;
    }

    bool backtrack(size_t current_pos) {
        if (current_pos == empty_cells_.size()) return true;

        // MRV: Find the empty cell with the fewest possibilities
        size_t best_pos = current_pos;
        int min_choices = 10;
        uint16_t best_mask = 0;

        for (size_t i = current_pos; i < empty_cells_.size(); ++i) {
            int idx = empty_cells_[i];
            uint16_t used = rows_[row_map_[idx]] | cols_[col_map_[idx]] | boxes_[box_map_[idx]];
            uint16_t possible = (~used) & 0x1FF;
            int count = __builtin_popcount(possible);
            
            if (count == 0) return false;
            if (count < min_choices) {
                min_choices = count;
                best_pos = i;
                best_mask = possible;
                if (count == 1) break; 
            }
        }

        // Swap the best cell to the current position to "fix" it for this branch
        int target_idx = empty_cells_[best_pos];
        std::swap(empty_cells_[current_pos], empty_cells_[best_pos]);

        while (best_mask) {
            int num_idx = __builtin_ctz(best_mask);
            int num = num_idx + 1;
            best_mask &= ~(1 << num_idx);

            // Directly update bitmasks to avoid redundant checks in set()
            uint16_t bit = 1 << num_idx;
            int r = row_map_[target_idx];
            int c = col_map_[target_idx];
            int b = box_map_[target_idx];

            board_[target_idx] = num;
            rows_[r] |= bit;
            cols_[c] |= bit;
            boxes_[b] |= bit;

            if (backtrack(current_pos + 1)) return true;

            rows_[r] &= ~bit;
            cols_[c] &= ~bit;
            boxes_[b] &= ~bit;
            board_[target_idx] = 0;
        }

        // Restore the swap for other branches
        std::swap(empty_cells_[current_pos], empty_cells_[best_pos]);
        return false;
    }
};

#endif
