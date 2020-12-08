#include <fstream>
#include <iostream>
#include <optional>
#include <unordered_set>
#include <vector>

std::vector<int> readFile();

std::optional<std::pair<int, int>> f1(const std::vector<int> &v, int sum)
{
    std::unordered_set<int> set;
    for (const auto &el : v) {
        if (auto x = set.find(sum - el); x != set.end()) {
            return std::make_pair(el, sum - el);
        }
        set.insert(el);
    }
    return std::nullopt;
}

std::optional<int> f2(const std::vector<int> &v, int sum)
{
    std::unordered_set<int> set;
    for (const auto &el : v) {
        if (auto y = f1(v, sum - el); y) {
            return y->first * y->second * el;
        }
    }
    return std::nullopt;
}

std::vector<int> readFile()
{
    std::ifstream input { "input.txt", std::ios::in };
    int n;
    input >> n;
    std::vector<int> nums(n);
    for (auto &el : nums)
        input >> el;
    return nums;
}

int main()
{
    const auto v = readFile();

    const auto r1 = f1(v, 2020);
    std::cout << r1->first * r1->second << std::endl;

    const auto r2 = f2(v, 2020);
    std::cout << f2(v, 2020).value() << std::endl;
}
