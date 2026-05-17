#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <array>
#include <bitset>
#include <optional>
#include <iterator>
#include <cstdint>
#include <type_traits>
#include <cctype>

class Revelio {
public:
    using Board = std::array<int, 81>;

    Revelio() {
        reset();
    }

    void reset() {
        board_.fill(0);
        rows_.fill(0);
        cols_.fill(0);
        boxes_.fill(0);
    }

    // Load from iterators
    template <typename It>
    bool load(It begin, It end) {
        reset();
        int i = 0;
        for (auto it = begin; it != end && i < 81; ++it, ++i) {
            int num;
            using val_type = std::decay_t<typename std::iterator_traits<It>::value_type>;
            if constexpr (std::is_same_v<val_type, char>) {
                if (isdigit(*it)) num = *it - '0';
                else if (*it == '.' || *it == '0') num = 0;
                else { --i; continue; } // skip
            } else {
                num = static_cast<int>(*it);
            }
            
            if (num < 0 || num > 9) return false;
            if (num != 0) {
                if (!set(i / 9, i % 9, num)) return false;
            }
        }
        return i == 81;
    }

    // Load from a string or any iterable of integers/chars
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
            if (isdigit(ch) || ch == '.') {
                values.push_back(ch);
            }
        }
        return load(values);
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

    static constexpr int getBox(int r, int c) {
        return (r / 3) * 3 + (c / 3);
    }

    bool set(int r, int c, int num) {
        int bit = 1 << (num - 1);
        int box = getBox(r, c);
        if ((rows_[r] & bit) || (cols_[c] & bit) || (boxes_[box] & bit)) {
            return false;
        }
        board_[r * 9 + c] = num;
        rows_[r] |= bit;
        cols_[c] |= bit;
        boxes_[box] |= bit;
        return true;
    }

    void unset(int r, int c, int num) {
        int bit = 1 << (num - 1);
        int box = getBox(r, c);
        board_[r * 9 + c] = 0;
        rows_[r] &= ~bit;
        cols_[c] &= ~bit;
        boxes_[box] &= ~bit;
    }

    bool backtrack(int index) {
        if (index == 81) return true;

        if (board_[index] != 0) {
            return backtrack(index + 1);
        }

        int r = index / 9;
        int c = index % 9;
        int box = getBox(r, c);
        uint16_t used = rows_[r] | cols_[c] | boxes_[box];

        for (int num = 1; num <= 9; ++num) {
            int bit = 1 << (num - 1);
            if (!(used & bit)) {
                board_[index] = num;
                rows_[r] |= bit;
                cols_[c] |= bit;
                boxes_[box] |= bit;

                if (backtrack(index + 1)) return true;

                rows_[r] &= ~bit;
                cols_[c] &= ~bit;
                boxes_[box] &= ~bit;
                board_[index] = 0;
            }
        }

        return false;
    }
};

int main(int argc, char* argv[]) {
    Revelio solver;
    if (argc > 1) {
        if (solver.loadFromFile(argv[1])) {
            std::cout << "Initial board:\n";
            solver.display();
            if (solver.revelio()) {
                std::cout << "\nSolved board:\n";
                solver.display();
            } else {
                std::cout << "\nNo solution found.\n";
            }
        } else {
            std::cerr << "Failed to load puzzle from " << argv[1] << "\n";
            return 1;
        }
    } else {
        std::cout << "Usage: " << argv[0] << " <sudoku_file>\n";
        std::cout << "Example puzzle (81 digits, use 0 or . for empty):\n";
        std::string example = "53..7....6..195....98....6.8...6...34..8.3..17...2...6.6....28....419..5....8..79";
        if (solver.load(example)) {
             if (solver.revelio()) {
                solver.display();
             }
        }
    }
    return 0;
}
