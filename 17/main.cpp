#include <iostream>

#include "boost/multi_array.hpp"
#include "boost/spirit/home/x3.hpp"
#include "utils.h"

using namespace boost::spirit;

struct Cell_ : x3::symbols<bool> {
    Cell_() { add("#", true)(".", false); }
};

using Space = boost::multi_array<bool, 4>;
using extents = Space::extent_gen;
using range = Space::extent_range;

constexpr auto N = 20;
constexpr auto IL = -N;
constexpr auto IR = N;

Space parseFile()
{
    auto row_ = +Cell_();
    auto rows_ = row_ % x3::eol;
    auto buffer = AOC::readFile("input.txt");

    std::vector<std::vector<bool>> value;
    auto result = x3::parse(buffer.begin(), buffer.end(), rows_, value);
    assert(result);

    auto extent = extents();
    auto space = Space(extent[range(-N, N)][range(-N, N)][range(-N, N)][range(-N, N)]);
    for (int i = 0; i < value.size(); ++i) {
        for (int j = 0; j < value[i].size(); ++j) { space[i][j][0][0] = value[i][j]; }
    }
    return space;
}

int neighborCount3(const Space &space, int m, int n, int o)
{
    int result = 0;
    for (int i = -1; i <= 1; ++i)
        for (int j = -1; j <= 1; ++j)
            for (int k = -1; k <= 1; ++k) { result += space[m + i][n + j][o + k][0]; }
    result -= space[m][n][o][0];
    return result;
}

int neighborCount4(const Space &space, int m, int n, int o, int p)
{
    int result = 0;
    for (int i = -1; i <= 1; ++i)
        for (int j = -1; j <= 1; ++j)
            for (int k = -1; k <= 1; ++k)
                for (int l = -1; l <= 1; ++l) { result += space[m + i][n + j][o + k][p + l]; }
    result -= space[m][n][o][p];
    return result;
}

auto applyRule1(const Space &space)
{
    auto to = space;
    const int start = IL + 1, end = IR - 1;
    for (int i = start; i < end; ++i) {
        for (int j = start; j < end; ++j) {
            for (int k = start; k < end; ++k) {
                auto nn = neighborCount3(space, i, j, k);
                if (space[i][j][k][0]) {
                    to[i][j][k][0] = nn == 2 || nn == 3;
                } else {
                    to[i][j][k][0] = nn == 3;
                }
            }
        }
    }
    return to;
}

auto applyRule2(const Space &space)
{
    auto to = space;
    const int start = IL + 1, end = IR - 1;
    for (int i = start; i < end; ++i) {
        for (int j = start; j < end; ++j) {
            for (int k = start; k < end; ++k) {
                for (int l = start; l < end; ++l) {
                    auto nn = neighborCount4(space, i, j, k, l);
                    if (space[i][j][k][l]) {
                        to[i][j][k][l] = nn == 2 || nn == 3;
                    } else {
                        to[i][j][k][l] = nn == 3;
                    }
                }
            }
        }
    }
    return to;
}

template <typename F> void f12(const Space &space, F f, int iterN)
{
    auto now = space;
    for (int iter = 0; iter < iterN; ++iter) { now = f(now); }

    int totalOn = 0;
    const int start = IL, end = IR;
    for (int i = start; i < end; ++i) {
        for (int j = start; j < end; ++j) {
            for (int k = start; k < end; ++k) {
                for (int l = start; l < end; ++l) { totalOn += now[i][j][k][l]; }
            }
        }
    }

    std::cout << totalOn << std::endl;
}

int main()
{
    const auto space = parseFile();
    f12(space, applyRule1, 6);
    f12(space, applyRule2, 6);
}