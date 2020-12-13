#include <iostream>

#include "boost/spirit/home/x3.hpp"

#include "utils.h"

std::vector<uint64_t> parseFile()
{
    using namespace boost::spirit;
    using x3::eol;
    using x3::uint64;
    const auto buffer = AOC::readFile("input.txt");
    std::vector<uint64_t> values;
    auto result = x3::parse(buffer.begin(), buffer.end(), uint64 % eol, values);
    std::cout << "Parser " << result << " with count " << values.size() << std::endl;
    return values;
}

template <typename Iterator, typename T> bool isSumPossible(Iterator begin, Iterator end, T t)
{
    auto l = begin;
    auto r = end - 1;
    while (l < r) {
        auto sum = *l + *r;
        if (sum < t) {
            ++l;
        } else if (sum > t) {
            --r;
        } else {
            return true;
        }
    }
    return false;
}

std::pair<size_t, uint64_t> f1(const std::vector<uint64_t> &data, uint64_t preamble)
{
    for (size_t i = preamble; i < data.size(); ++i) {
        const auto preambleBegin = data.begin() + (i - preamble);
        auto window = std::vector(preambleBegin, preambleBegin + preamble);
        std::sort(window.begin(), window.end());
        if (auto valid = isSumPossible(window.begin(), window.end(), data[i]); !valid) {
            std::cout << i << '\t' << data[i] << '\t' << valid << std::endl;
            return { i, data[i] };
        }
    }
    throw std::runtime_error { "No invalid numbers found" };
}

uint64_t f2(const std::vector<uint64_t> &data, uint64_t preamble)
{
    const auto [invalidIdx, invalidNum] = f1(data, preamble);
    const auto beg = data.begin();
    const auto end = data.begin() + invalidIdx;
    for (auto it = beg; it != end; ++it) {
        auto my_it = it;
        uint64_t total = 0;
        do {
            total += *my_it++;
        } while (total < invalidNum);
        if (total == invalidNum) {
            auto [min, max] = std::minmax_element(it, my_it);
            return *min + *max;
        }
    }
    throw std::runtime_error { "No contiguous range found" };
}

int main()
{
    auto input = parseFile();
    auto f1_result = f1(input, 25);
    std::cout << "Invalid number " << f1_result.second << std::endl;
    auto f2_result = f2(input, 25);
    std::cout << "Weakness " << f2_result << std::endl;
}