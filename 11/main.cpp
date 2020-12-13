#include <iostream>

#include "boost/spirit/home/x3.hpp"

#include "utils.h"

enum class Position {
    Floor,
    Empty,
    Occupied,
};

struct Position_ : boost::spirit::x3::symbols<Position> {
    Position_() { add(".", Position::Floor)("L", Position::Empty)("#", Position::Occupied); }
};

std::vector<std::vector<Position>> parseFile()
{
    using namespace boost::spirit::x3;
    const auto buffer = AOC::readFile("input.txt");
    auto position_ = Position_();
    auto row_ = +position_;
    std::vector<std::vector<Position>> values;
    auto result = parse(buffer.begin(), buffer.end(), row_ % eol, values);
    assert(result);
    std::cout << "Found " << values.size() << " rows" << std::endl;
    return values;
}

auto ruleSet1(const std::vector<std::vector<Position>> &data, size_t n)
{
    auto current = data;
    const auto countAdjacents = [](auto &input, int i, int j, int n) {
        int total = 0;
        const auto l = 0, r = (n - 1);
        const auto key = Position::Occupied;
        total += (i > l && input[i - 1][j] == key);
        total += (j > l && input[i][j - 1] == key);
        total += (i < r && input[i + 1][j] == key);
        total += (j < r && input[i][j + 1] == key);
        total += (i > l && j > l && input[i - 1][j - 1] == key);
        total += (i > l && j < r && input[i - 1][j + 1] == key);
        total += (i < r && j > l && input[i + 1][j - 1] == key);
        total += (i < r && j < r && input[i + 1][j + 1] == key);
        return total;
    };
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            switch (data[i][j]) {
            case Position::Empty: {
                if (countAdjacents(data, i, j, n) == 0) current[i][j] = Position::Occupied;
                break;
            }
            case Position::Occupied: {
                if (countAdjacents(data, i, j, n) >= 4) current[i][j] = Position::Empty;
                break;
            }
            case Position::Floor:
                break;
            }
        }
    }
    return current;
}

auto ruleSet2(const std::vector<std::vector<Position>> &data, size_t n)
{
    auto current = data;
    const auto countAdjacents = [](const auto &input, int i, int j, int size) {
        int total = 0;
        const int l = 0;
        const int r = (size - 1);

        const std::vector<std::pair<int, int>> axes = {
            std::pair { 1, 0 },
            std::pair { 0, 1 },
            std::pair { -1, 0 },
            std::pair { 0, -1 },
            std::pair { 1, 1 },
            std::pair { 1, -1 },
            std::pair { -1, 1 },
            std::pair { -1, -1 },
        };

        for (auto [moff, noff] : axes) {
            for (int m = i + moff, n = j + noff; ((m >= l) && (m <= r)) && ((n >= l) && (n <= r)); m += moff, n += noff) {
                if (input[m][n] != Position::Floor) {
                    total += input[m][n] == Position::Occupied;
                    break;
                }
            }
        }

        return total;
    };
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            switch (data[i][j]) {
            case Position::Empty: {
                if (countAdjacents(data, i, j, n) == 0) current[i][j] = Position::Occupied;
                break;
            }
            case Position::Occupied: {
                if (countAdjacents(data, i, j, n) >= 5) current[i][j] = Position::Empty;
                break;
            }
            case Position::Floor:
                break;
            }
        }
    }
    return current;
}

template <typename RuleSet> int f12(const std::vector<std::vector<Position>> &data, size_t n, const RuleSet &ruleSet)
{
    auto current = data;
    auto next = ruleSet(data, n);
    while (next != current) { current = std::exchange(next, ruleSet(next, n)); }

    auto totalOccupied = 0;
    for (const auto &row : current) {
        for (const auto &item : row) { totalOccupied += item == Position::Occupied; }
    }
    return totalOccupied;
}

int main()
{
    const auto data = parseFile();
    const auto n = data.size();
    std::cout << f12(data, n, ruleSet1) << std::endl;
    std::cout << f12(data, n, ruleSet2) << std::endl;
}