#include <iostream>
#include <numeric>

#include "boost/spirit/home/x3.hpp"

#include "utils.h"

std::vector<int> parseFile()
{
    using namespace boost::spirit::x3;
    const auto buffer = AOC::readFile("input.txt");
    std::vector<int> values;
    auto result = phrase_parse(buffer.begin(), buffer.end(), +int_, eol, values);
    assert(result);
    std::cout << "Read count " << values.size() << std::endl;
    return values;
}

long f1(std::vector<int> data)
{
    std::sort(data.begin(), data.end());
    data.emplace_back(data.back() + 3);
    std::vector<int> diff;
    std::adjacent_difference(data.begin(), data.end(), std::back_inserter(diff));
    return std::count(diff.begin(), diff.end(), 1) * std::count(diff.begin(), diff.end(), 3);
}

// Tree implementation is super slow, duh!
//
// template <typename Iterator>
// int f2_impl(Iterator it, Iterator end, int prev, int limit) {
//     if (*it > 3 + prev) return 0;
//     if (it == end) return 1;
//     return f2_impl(it + 1, end, *it, limit) + f2_impl(it + 1, end, prev, limit);
// }
//
// long f2(std::vector<int> data)
// {
//     std::sort(data.begin(), data.end());
//     data.emplace_back(data.back() + 3);
//     return f2_impl(data.begin(), data.end() - 2, 0, data.back());
// }

uint64_t f2(std::vector<int> data)
{
    data.insert(data.begin(), 0);
    std::sort(data.begin(), data.end());
    std::vector<uint64_t> counts(data.size(), 0);

    counts.front() = 1;
    for (int i = 1; i < data.size(); ++i) {
        int cur = i - 1;
        while (cur >= 0 && (data[i] - data[cur]) < 4) { counts[i] += counts[cur--]; }
    }
    return counts.back();
}

int main()
{
    const auto data = parseFile();
    std::cout << f1(data) << std::endl;
    std::cout << f2(data) << std::endl;
}