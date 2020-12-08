#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct Pass {
    int from;
    int to;
    char c;
    std::string pass;
};

std::ostream &operator<<(std::ostream &out, const Pass &pass)
{
    out << pass.from << '-' << pass.to << ' ' << pass.c << ": " << pass.pass;
    return out;
}

int readNum(std::istream &in)
{
    int r = 0;
    do {
        r = r * 10 + (in.get() - '0');
    } while (isdigit(in.peek()));
    return r;
}

std::vector<Pass> read()
{
    std::ifstream in { "input.txt" };
    int n;
    in >> n;
    in.get(); // _:
    std::vector<Pass> v(n);
    for (auto &p : v) {
        p.from = readNum(in);
        in.get(); // -
        p.to = readNum(in);
        in.get(); // _
        in >> p.c;
        in.get(); // :
        in.get(); // _:
        in >> p.pass;
        in.get(); // _:
    }
    return v;
}

int f1(const std::vector<Pass> &v, int n)
{
    int res = 0;
    for (const auto &e : v) {
        auto count = std::count(e.pass.begin(), e.pass.end(), e.c);
        if (count >= e.from && count <= e.to)
            ++res;
    }
    return res;
}

int f2(const std::vector<Pass> &v, int n)
{
    int res = 0;
    for (const auto &e : v) {
        res += ((e.pass[e.from - 1] == e.c) ^ (e.pass[e.to - 1] == e.c));
    }
    return res;
}

int main()
{
    const auto v = read();
    std::cout << f1(v, v.size()) << std::endl;
    std::cout << f2(v, v.size()) << std::endl;
}
