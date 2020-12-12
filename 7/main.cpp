#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#include "boost/fusion/adapted.hpp"
#include "boost/spirit/home/x3.hpp"

std::vector<char> readFile()
{
    std::ifstream in { "input.txt", std::ios::ate };
    std::vector<char> buffer(in.tellg());
    in.seekg(std::ios::beg);
    in.read(buffer.data(), buffer.size());
    return buffer;
}

using color_type = std::string;
using child_type = std::pair<int, color_type>;
using rule_type = boost::tuple<color_type, std::vector<child_type>>;
using rules_type = std::unordered_map<color_type, std::vector<child_type>>;

std::ostream &operator<<(std::ostream &out, const child_type &child)
{
    out << child.first << "@" << child.second;
    return out;
}

std::ostream &operator<<(std::ostream &out, const rule_type &rule)
{
    out << rule.get<0>() << ": ";
    for (const auto &item : rule.get<1>()) out << item << " ";
    return out;
}

template <typename Iterator> auto parseBuffer(Iterator begin, Iterator end)
{
    using namespace boost::spirit;
    using x3::alpha;
    using x3::eol;
    using x3::int_;
    using x3::lexeme;
    using x3::lit;
    using x3::omit;
    using x3::rule;
    using x3::ascii::space;

    auto word_ = lexeme[+alpha];

    struct color_tag;
    auto color_ = rule<color_tag, color_type>() = word_ >> word_;

    auto bag_ = color_ >> (lit("bags") | lit("bag"));

    struct child_tag;
    auto child_ = rule<child_tag, child_type>() = (int_ >> bag_);

    struct rule_tag;
    auto no_bags_ = lit("no other bags");
    auto rule_ = rule<rule_tag, rule_type>() = bag_ >> lit("contain") >> ((child_ % ",") | no_bags_) >> ".";
    auto rules_ = +rule_;

    rules_type value;
    auto result = x3::phrase_parse(begin, end, rules_, space, value);
    std::cout << "Parser " << (result ? "success" : "failure") << " at " << std::string(begin, begin) << std::endl;
    std::cout << "Read " << value.size() << " rules" << std::endl;
    return value;
}

void f1(const rules_type &rules, const std::string &color)
{
    std::unordered_set<color_type> parents;
    std::queue<color_type> queue;
    queue.push(color);
    while (!queue.empty()) {
        auto current = queue.front();
        for (const auto &[parent, children] : rules) {
            for (const auto &[count, child] : children) {
                if (child == current) {
                    parents.insert(parent);
                    queue.push(parent);
                }
            }
        }
        queue.pop();
    }
    std::cout << "Parents: " << parents.size() << std::endl;
}

int f2_impl(const rules_type &rules, const color_type &color, std::unordered_map<color_type, int> &memoizer)
{
    int total = 0;
    for (const auto &[count, child] : rules.at(color)) {
        int to_add;
        if (auto it = memoizer.find(child); it != memoizer.end()) {
            to_add = it->second;
        } else {
            to_add = f2_impl(rules, child, memoizer);
            memoizer[child] = to_add;
        }
        total += count * (1 + to_add);
    }
    return total;
}

void f2(const rules_type &rules, const color_type &color)
{
    std::unordered_map<color_type, int> cache;
    std::cout << "Children: " << f2_impl(rules, color, cache) << std::endl;
}

int main()
{
    const auto buffer = readFile();
    auto rules = parseBuffer(buffer.begin(), buffer.end());
    f1(rules, "shinygold");
    f2(rules, "shinygold");
}