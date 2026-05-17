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

class Revelio {
public:
    using Board = std::array<int, 81>;

    Revelio() {
        for (int i = 0; i < 81; ++i) {
            int r = i / 9;
            int c = i % 9;
            int b = (r / 3) * 3 + (c / 3);
            row_map_[i] = r;
            col_map_[i] = c;
            box_map_[i] = b;

            // Precompute neighbors (max 20 unique cells per cell)
            std::array<int, 20> n_list;
            int count = 0;
            for (int j = 0; j < 81; ++j) {
                if (i == j) continue;
                int jr = j / 9, jc = j % 9, jb = (jr / 3) * 3 + (jc / 3);
                if (r == jr || c == jc || b == jb) {
                    n_list[count++] = j;
                }
            }
            neighbor_count_[i] = count;
            neighbors_[i] = n_list;
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
        std::array<int, 81> initial;
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
            initial[i] = num;
        }
        if (i != 81) return false;

        for (int idx = 0; idx < 81; ++idx) {
            if (initial[idx] != 0) {
                if (!set(idx, initial[idx])) return false;
            } else {
                empty_cells_.push_back(idx);
            }
        }
        // Initial candidates calculation
        for (int idx : empty_cells_) {
            uint16_t used = rows_[row_map_[idx]] | cols_[col_map_[idx]] | boxes_[box_map_[idx]];
            candidates_[idx] = (~used) & 0x1FF;
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
        std::string line;
        if (std::getline(file, line)) return load(line);
        return false;
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
    std::array<uint16_t, 81> candidates_;
    std::array<int, 81> row_map_, col_map_, box_map_;
    std::array<std::array<int, 20>, 81> neighbors_;
    std::array<int, 81> neighbor_count_;
    std::vector<int> empty_cells_;

    inline bool set(int idx, int num) {
        uint16_t bit = 1 << (num - 1);
        int r = row_map_[idx], c = col_map_[idx], b = box_map_[idx];
        if ((rows_[r] & bit) || (cols_[c] & bit) || (boxes_[b] & bit)) return false;
        board_[idx] = num;
        rows_[r] |= bit;
        cols_[c] |= bit;
        boxes_[b] |= bit;
        return true;
    }

    bool backtrack(size_t current_pos) {
        if (current_pos == empty_cells_.size()) return true;

        size_t best_pos = current_pos;
        int min_choices = 10;

        for (size_t i = current_pos; i < empty_cells_.size(); ++i) {
            int idx = empty_cells_[i];
            // Compute candidates on the fly but use the neighbor-optimized bitmasks
            uint16_t used = rows_[row_map_[idx]] | cols_[col_map_[idx]] | boxes_[box_map_[idx]];
            uint16_t possible = (~used) & 0x1FF;
            int count = __builtin_popcount(possible);
            
            if (count == 0) return false;
            if (count < min_choices) {
                min_choices = count;
                best_pos = i;
                if (count == 1) break;
            }
        }

        int target_idx = empty_cells_[best_pos];
        std::swap(empty_cells_[current_pos], empty_cells_[best_pos]);

        uint16_t mask = (~(rows_[row_map_[target_idx]] | cols_[col_map_[target_idx]] | boxes_[box_map_[target_idx]])) & 0x1FF;
        
        while (mask) {
            int num_idx = __builtin_ctz(mask);
            uint16_t bit = 1 << num_idx;
            mask &= ~bit;

            int r = row_map_[target_idx], c = col_map_[target_idx], b = box_map_[target_idx];
            board_[target_idx] = num_idx + 1;
            rows_[r] |= bit;
            cols_[c] |= bit;
            boxes_[b] |= bit;

            if (backtrack(current_pos + 1)) return true;

            rows_[r] &= ~bit;
            cols_[c] &= ~bit;
            boxes_[b] &= ~bit;
            board_[target_idx] = 0;
        }

        std::swap(empty_cells_[current_pos], empty_cells_[best_pos]);
        return false;
    }
};

#endif
