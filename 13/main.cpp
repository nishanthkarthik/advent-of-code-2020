#include <iostream>
#include <numeric>

#include "boost/fusion/adapted.hpp"
#include "boost/spirit/home/x3.hpp"

#include "utils.h"

using input_type = std::pair<int, std::vector<int>>;

input_type parseFile()
{
    const auto buffer = AOC::readFile("input.txt");
    using namespace boost::spirit::x3;
    input_type values;
    const auto insertZero = [&](auto &ctx) { values.second.emplace_back(0); };
    auto first_ = int_;
    auto second_ = (int_ | omit[lit("x")][insertZero]) % ",";
    const auto result = parse(buffer.begin(), buffer.end(), first_ >> eol >> second_, values);
    assert(result);
    return values;
}

int f1(const input_type &input)
{
    auto min = std::numeric_limits<int>::max();
    int busId {};
    const auto depart = input.first;
    for (auto bus : input.second) {
        if (!bus) continue;
        if (auto delta = (bus - depart % bus); delta < min) {
            min = delta;
            busId = bus;
        }
    }
    return busId * (busId - depart % busId);
}

uint64_t f2(const input_type &input)
{
    auto &v = input.second;
    uint64_t t = 0;
    uint64_t step = v.front();
    for (int i = 1; i < v.size(); ++i) {
        if (!v[i]) continue;
        while (true) {
            if ((t + i) % v[i] == 0) break;
            t += step;
        }
        step = std::lcm(step, v[i]);
    }
    return t;
}

int main()
{
    const auto input = parseFile();
    std::cout << f1(input) << std::endl;
    std::cout << f2(input) << std::endl;
}