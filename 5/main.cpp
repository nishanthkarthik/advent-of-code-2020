#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>
#include <cassert>
#include <string>
#include <vector>

std::vector<std::string> readFile()
{
    std::ifstream in { "input.txt" };
    int n;
    in >> n;
    std::vector<std::string> v(n);
    for (auto &e : v) in >> e;
    return v;
}

template <typename Iterator, size_t Length> size_t decodeString(Iterator begin, char l)
{
    std::bitset<Length> bits;
    for (auto it = begin; it != begin + Length; ++it) bits[Length - 1 - (it - begin)] = *it != l;
    return bits.to_ulong();
}

void f12(const std::vector<std::string> &v)
{
    std::vector<size_t> list;
    list.reserve(v.size());
    for (auto &e : v) {
        using iterator = decltype(e.begin());
        auto row = decodeString<iterator, 7>(e.begin(), 'F');
        auto col = decodeString<iterator, 3>(e.begin() + 7, 'L');
        auto uid = row * 8 + col;
        list.emplace_back(uid);
    }
    std::sort(list.begin(), list.end());
    auto min = list.front();
    auto max = list[list.size() - 1];
    std::cout << "Max UID " << max << std::endl;

    size_t bits = 0;
    for (size_t i = min; i <= max; ++i) bits ^= i;
    for (auto e : list) bits ^= e;
    std::cout << "Missing UID between " << min << ", " << max << ": " << bits << std::endl;
    assert(std::find(list.begin(), list.end(), bits) == list.end());
}

int main()
{
    auto v = readFile();
    f12(v);
}