#include <iostream>
#include <map>
#include <set>
#include <unordered_set>

#include "boost/fusion/adapted.hpp"
#include "boost/spirit/home/x3.hpp"

#include "utils.h"

struct TicketField {
    std::string name;
    std::pair<int, int> l, r;
};

struct Input {
    std::vector<TicketField> field;
    std::vector<int> ticket;
    std::vector<std::vector<int>> nearby;
};

BOOST_FUSION_ADAPT_STRUCT(TicketField, name, l.first, l.second, r.first, r.second);
BOOST_FUSION_ADAPT_STRUCT(Input, field, ticket, nearby);

auto parseFile()
{
    using namespace boost::spirit::x3;
    auto key_ = rule<struct TicketField, TicketField>()
        = +(alpha | space) >> ": " >> int_ >> "-" >> int_ >> " or " >> int_ >> "-" >> int_;
    auto keys_ = key_ % eol;
    auto your_ticket_ = rule<struct YourTicketField, std::vector<int>>() = "your ticket:" >> eol >> (int_ % ",");
    auto nearby_ = rule<struct NearbyField, std::vector<std::vector<int>>>()
        = "nearby tickets:" >> eol >> (int_ % ",") % eol;
    auto input_ = rule<struct InputField, Input>() = keys_ >> +eol >> your_ticket_ >> +eol >> nearby_;
    auto buffer = AOC::readFile("input.txt");
    Input input;
    auto it = buffer.begin();
    auto result = parse(it, buffer.end(), input_, input);
    assert(result);
    return input;
}

long checkAllValid(const std::vector<TicketField> &field, const std::vector<int> &ticket)
{
    long t = 0;
    for (const auto &item : ticket) {
        bool okay = false;
        for (auto [name, l, r] : field) {
            if (item == std::clamp(item, l.first, l.second) || item == std::clamp(item, r.first, r.second)) {
                okay = true;
                break;
            }
        }
        if (!okay) t += item;
    }
    return t;
}

auto f1(const Input &input)
{
    long t = 0;
    for (const auto &row : input.nearby) t += checkAllValid(input.field, row);
    return t;
}

constexpr bool withinRange(int val, std::pair<int, int> l, std::pair<int, int> r)
{
    return val == std::clamp(val, l.first, l.second) || val == std::clamp(val, r.first, r.second);
}

auto f2(const Input &input)
{
    auto nearby = std::vector<std::vector<int>>();
    std::copy_if(input.nearby.begin(), input.nearby.end(), std::back_inserter(nearby),
        [&](const auto &item) { return checkAllValid(input.field, item) == 0; });
    nearby.emplace_back(input.ticket);

    const auto cols = input.ticket.size();
    const auto rows = nearby.size();
    std::vector<std::set<int>> coldata(cols);
    for (int i = 0; i < cols; ++i) {
        for (int j = 0; j < rows; ++j) { coldata[i].insert(nearby[j][i]); }
    }

    std::map<int, std::unordered_set<int>> pos;
    for (int i = 0; i < cols; ++i) {
        const auto &col = coldata[i];
        auto min = *col.begin();
        auto max = *col.rbegin();
        for (int j = 0; j < input.field.size(); ++j) {
            const auto &[name, l, r] = input.field[j];
            if (!withinRange(min, l, r)) continue;
            if (!withinRange(max, l, r)) continue;
            const auto lr = std::upper_bound(col.begin(), col.end(), l.second);
            if (lr != col.end() && *lr != std::clamp(*lr, r.first, r.second)) continue;
            pos[i].insert(j);
        }
    }

    const auto getOneValueKey = [&pos] {
        return std::find_if(pos.begin(), pos.end(), [](const auto &item) { return item.second.size() == 1; });
    };
    std::unordered_map<int, int> mapping;
    while (!pos.empty()) {
        auto it = getOneValueKey();
        assert(it != pos.end());
        auto keyToAdd = it->first;
        auto valueToRemove = *it->second.begin();
        pos.erase(keyToAdd);
        for (auto &[key, val] : pos) { val.erase(valueToRemove); }
        mapping[valueToRemove] = keyToAdd;
        std::cout << keyToAdd << " -> " << valueToRemove << std::endl;
    }

    uint64_t product = 1;
    for (int i = 0; i < 6; ++i) {
        product *= input.ticket[mapping[i]];
    }
    return product;
}

int main()
{
    const auto input = parseFile();
    std::cout << "f1 " << f1(input) << std::endl;
    std::cout << f2(input) << std::endl;
}