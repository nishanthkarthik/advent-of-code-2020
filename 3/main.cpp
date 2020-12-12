#include <fstream>
#include <iostream>
#include <vector>

std::vector<std::string> readInput()
{
    std::ifstream in { "input.txt" };
    int n;
    in >> n;
    std::vector<std::string> r(n);
    for (auto &e : r)
        in >> e;
    return r;
}

int nTrees(const std::vector<std::string> &v, int n, int right, int down)
{
    int res = 0;
    int col = right;
    for (int i = down; i < n; i += down) {
        col %= v[i].size();
        res += v[i][col] == '#';
        col += right;
    }
    return res;
}

int main()
{
    const auto v = readInput();
    const long r1 = nTrees(v, v.size(), 1, 1);
    std::cout << r1 << std::endl;

    const long r2 = nTrees(v, v.size(), 3, 1);
    std::cout << r2 << std::endl;

    const long r3 = nTrees(v, v.size(), 5, 1);
    std::cout << r3 << std::endl;

    const long r4 = nTrees(v, v.size(), 7, 1);
    std::cout << r4 << std::endl;

    const long r5 = nTrees(v, v.size(), 1, 2);
    std::cout << r5 << std::endl;

    std::cout << (r1 * r2 * r3 * r4 * r5) << std::endl;
}
