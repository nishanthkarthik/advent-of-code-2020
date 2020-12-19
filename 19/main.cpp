#include "common.h"

#include "boost/algorithm/string.hpp"
#include "boost/format.hpp"

namespace typing {
using str_type = char;
using seq_type = std::vector<int>;
using seqs_type = std::vector<seq_type>;
using value_type = boost::variant<str_type, seqs_type>;
using rule_type = boost::tuple<int, value_type>;
using rules_type = std::unordered_map<int, value_type>;
using input_type = std::vector<std::string>;
}

auto parseFile()
{
    using namespace boost::spirit::x3;
    auto str_ = rule<struct str_, typing::str_type>() = "\"" >> alpha >> "\"";
    auto seq_ = rule<struct seq_, typing::seq_type>() = int_ % " ";
    auto seqs_ = rule<struct seqs_, typing::seqs_type>() = seq_ % " | ";
    auto value_ = rule<struct rule_, typing::value_type>() = seqs_ | str_;
    auto rule_ = rule<struct rule_, typing::rule_type>() = int_ >> ": " >> value_;
    auto rules_ = rule_ % eol;
    typing::rules_type rules;
    const auto buffer = AOC::readFile("input.txt");
    auto it = buffer.begin();
    auto res = parse(it, buffer.end(), rules_, rules);
    if (it - buffer.end() > 5) std::cout << std::string(it, it + 5) << std::endl;
    assert(res);

    typing::input_type inputs;
    res = parse(it, buffer.end(), omit[*eol] >> +alpha % eol, inputs);
    if (it - buffer.end() > 3) std::cout << std::string(it, it + 3) << std::endl;
    assert(res);
    return std::make_pair(rules, inputs);
}

// Spirit codegen does not work because boost spirit does not do exhaustive backtracking
// https://www.boost.org/doc/libs/1_35_0/libs/spirit/doc/rationale.html#exhaustive_rd
auto f1Gen(const typing::rules_type &rules, std::ostream &out)
{
    for (const auto &[idx, _] : rules) { out << boost::str(boost::format("x3::rule<struct t%1%> rule%1%;\n") % idx); }

    for (const auto &[idx, rule] : rules) {
        boost::apply_visitor(
            [idx = idx, &out](const auto &it) {
                using T = std::decay_t<decltype(it)>;
                if constexpr (std::is_same_v<T, typing::str_type>) {
                    out << boost::str(boost::format("auto rule%1%_def = x3::char_(\"%2%\");\n") % idx % it);
                } else {
                    out << boost::str(boost::format("auto rule%1%_def = ") % idx);
                    std::vector<std::string> seqs;
                    std::transform(it.begin(), it.end(), std::back_inserter(seqs), [](const auto &row) {
                        std::vector<std::string> items;
                        std::transform(row.begin(), row.end(), std::back_inserter(items),
                            [](const auto &item) { return "rule" + std::to_string(item); });
                        using boost::algorithm::join;
                        return "( " + join(items, " >> ") + " )";
                    });
                    out << boost::algorithm::join(seqs, " | ") << ';' << std::endl;
                }
                out << boost::str(boost::format("BOOST_SPIRIT_DEFINE(rule%1%)") % idx) << std::endl;
            },
            rule);
    }
}
bool f1(const std::string &input);

bool f1_x(std::string input, std::vector<int> seq, const typing::rules_type &rules)
{
    if (input.empty() || seq.empty()) return input.empty() && seq.empty();
    const auto &rule = rules.at(seq.front());
    if (rule.which() == 0) {
        if (input.front() == boost::get<typing::str_type>(rule)) {
            return f1_x(std::string(input.begin() + 1, input.end()), std::vector(seq.begin() + 1, seq.end()), rules);
        } else {
            return false;
        }
    } else {
        const auto &rr = boost::get<typing::seqs_type>(rule);
        return std::any_of(rr.begin(), rr.end(), [&](auto it) {
            it.insert(it.end(), seq.begin() + 1, seq.end());
            return f1_x(input, it, rules);
        });
    }
}

int main()
{
    const auto [rules, inputs] = parseFile();
    constexpr auto enableCodegen = 0;
    if constexpr (enableCodegen) {
        f1Gen(rules, std::cout);
    } else {
        auto total = 0;
        for (const auto &input : inputs) {
            auto value = f1_x(input, { 0 }, rules);
            // std::cout << input << " " << value << std::endl;
            total += value;
        }
        std::cout << "total1 " << total << std::endl;
    }
}