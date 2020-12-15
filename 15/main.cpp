#include <iostream>
#include <unordered_map>

#include "boost/spirit/home/x3.hpp"

#include "utils.h"

using T = std::vector<int>;

T parseFile()
{
    auto buffer = AOC::readFile("input.txt");
    std::vector<int> r;
    using namespace boost::spirit::x3;
    auto result = parse(buffer.begin(), buffer.end(), int_ % ",", r);
    assert(result);
    return r;
}

int f1(const T &t, int turnCount)
{
    std::unordered_map<int, int> count;
    std::unordered_map<int, int> most;
    std::unordered_map<int, int> more;

    for (int i = 0; i < t.size(); ++i) {
        ++count[t[i]];
        most[t[i]] = i;
    }

    int last = t.back();

    for (int i = t.size(); i < turnCount + t.size(); ++i) {
        if (count[last] <= 1) {
            last = 0;
        } else {
            last = most[last] - more[last];
        }
        ++count[last];
        more[last] = most[last];
        most[last] = i;
    }
    return last;
}

int main() {
    const auto input = parseFile();
    const long iter = 30000000;
    std::cout << f1(input, iter - input.size()) << std::endl;
}