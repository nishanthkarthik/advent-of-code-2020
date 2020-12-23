#include "common.h"

#include "boost/pool/pool_alloc.hpp"

#include <list>

template <typename T> std::ostream &operator<<(std::ostream &out, const std::list<T> &t)
{
    for (const auto e : t) out << +e << ' ';
    return out;
}

template <typename T, size_t N> std::ostream &operator<<(std::ostream &out, const std::array<T, N> &t)
{
    for (const auto e : t) out << +e << ' ';
    return out;
}

template <typename T> std::ostream &operator<<(std::ostream &out, const std::vector<T> &t)
{
    for (const auto e : t) out << +e << ' ';
    return out;
}

template <typename T> struct List {
    explicit List(const std::list<T> &list)
    {
        mem.resize(list.size() + 1);
        for (auto it = list.begin(); std::next(it) != list.end(); ++it) { mem[*it] = *std::next(it); }
        mem[*std::prev(list.end())] = *list.begin();
        start = *list.begin();
    }

    T popAfter(T t)
    {
        T ret = mem[t];
        mem[t] = mem[mem[t]];
        return ret;
    }

    void pushAfter(T t, T value)
    {
        T toConnect = mem[t];
        mem[t] = value;
        mem[value] = toConnect;
    }

    T peekAfter(T t) const { return mem[t]; }

    typedef T value_type;
    typedef T iterator;
    std::vector<T> mem;
    T start;
};

template <typename T> std::ostream &operator<<(std::ostream &out, const List<T> &t)
{
    auto now = t.start;
    int count = 0;
    do {
        std::cout << now << ' ';
        now = t.peekAfter(now);
    } while (now != t.start && count++ < t.mem.size() - 1);
    return out;
}

using T = List<int32_t>;

auto parseFile()
{
    using namespace boost::spirit::x3;
    std::list<T::value_type> list;
    auto parser_ = int_parser<T::value_type, 10, 1, 1>();
    const auto buffer = AOC::readFile("input.txt");
    auto res = parse(buffer.begin(), buffer.end(), +parser_, list);
    assert(res);
    return list;
}

template <typename T, size_t N, size_t... Idxs>
bool array_not_has_impl(const std::array<T, N> &haystack, T needle, std::index_sequence<Idxs...>)
{
    return ((haystack[Idxs] != needle) && ...);
}

template <typename T, size_t N> bool array_not_has(const std::array<T, N> &haystack, T needle)
{
    return array_not_has_impl(haystack, needle, std::make_index_sequence<N>());
}

inline T::iterator f1_turn(T &t, T::iterator start)
{
    constexpr auto pickCount = 3;
    std::array<T::value_type, pickCount> selected {};

    for (int i = 0; i < pickCount; ++i) { selected[i] = t.popAfter(start); }

    // std::cout << '\t' << "T : " << t << std::endl;
    // std::cout << '\t' << "Select " << selected << std::endl;

    T::value_type target = start - 1;
    while (target > 0) {
        if (std::find(selected.begin(), selected.end(), target) == selected.end()) break;
        if (array_not_has(selected, target)) break;
        --target;
    }

    if (target == 0) {
        target = t.mem.size() - 1;
        do {
            // std::cout << '\t' << "target " << target << std::endl;
            if (array_not_has(selected, target)) break;
            --target;
        } while (target >= 0);
    }
    // std::cout << '\t' << "Dest " << target << std::endl;

    t.pushAfter(target, selected[0]);
    t.pushAfter(selected[0], selected[1]);
    t.pushAfter(selected[1], selected[2]);
    return t.peekAfter(start);
}

auto f1(T t, int turns, T::value_type start)
{
    auto pick = start;
    for (int i = 0; i < turns; ++i) {
        // std::cout << "(" << pick << "): " << t << std::endl;
        pick = f1_turn(t, pick);
    }
    // std::cout << "Pick " << pick << std::endl;
    return t;
}

size_t f2(std::list<T::value_type> t)
{
    const auto n = t.size();
    constexpr auto moves = 10'000'000;
    for (int i = 0; i < 1'000'000 - n; ++i) { t.emplace_back(i + n + 1); }
    const auto res = f1(T { t }, moves, t.front());
    std::pair<size_t, size_t> stars{res.peekAfter(1), res.peekAfter(res.peekAfter(1))};
    std::cout << "After " << stars.first << ' ' << stars.second << std::endl;
    return stars.first * stars.second;
}

int main()
{
    const auto in = parseFile();
    const auto r1 = f1(T { in }, 100, in.front());
    std::cout << "Final list " << r1 << std::endl;
    const auto r2 = f2(in);
    std::cout << "Stars product " << r2 << std::endl;
}