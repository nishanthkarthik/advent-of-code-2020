#include <iostream>
#include <set>

#include "boost/fusion/adapted.hpp"
#include "boost/spirit/home/x3.hpp"
#include "boost/variant.hpp"

#include "utils.h"

namespace ast {
struct MemorySet {
    uint64_t index;
    uint64_t value;
};

enum MaskBit {
    Zero,
    One,
    Nothing,
};

constexpr auto mask_len = 36;
using mask_type = std::vector<MaskBit>;
using instr_type = std::vector<boost::variant<ast::mask_type, ast::MemorySet>>;

std::ostream &operator<<(std::ostream &out, const mask_type &mask)
{
    for (const auto &e : mask) {
        switch (e) {
        case MaskBit::Nothing: out << 'X'; break;
        case MaskBit::Zero: out << '0'; break;
        case MaskBit::One: out << '1'; break;
        }
    }
    return out;
}

std::ostream &operator<<(std::ostream &out, const instr_type &instr)
{
    for (const auto &item : instr) {
        boost::apply_visitor(
            [&out](const auto &x) {
                using T = std::decay_t<decltype(x)>;
                if constexpr (std::is_same_v<T, mask_type>) {
                    out << x;
                } else {
                    out << x.index << " -> " << x.value;
                }
            },
            item);
        out << std::endl;
    }
    return out;
}
}

BOOST_FUSION_ADAPT_STRUCT(ast::MemorySet, index, value);

namespace parser {
using boost::spirit::x3::lit;
using boost::spirit::x3::rule;
using boost::spirit::x3::uint64;

rule<struct memset, ast::MemorySet> memory_set = "Memset";
auto memory_set_def = lit("mem[") >> uint64 >> "] = " >> uint64;
BOOST_SPIRIT_DEFINE(memory_set);

struct MaskBit_ : ::boost::spirit::x3::symbols<ast::MaskBit> {
    MaskBit_() { add("X", ast::MaskBit::Nothing)("0", ast::MaskBit::Zero)("1", ast::MaskBit::One); }
};
}

ast::instr_type parseFile()
{
    using namespace boost::spirit;
    using x3::eol;
    using x3::lit;
    using x3::repeat;
    using x3::rule;

    auto mask_ = lit("mask = ") >> repeat(36)[parser::MaskBit_()];
    auto line_ = (mask_ | parser::memory_set);
    auto lines_ = line_ % eol;

    const auto buffer = AOC::readFile("input.txt");

    ast::instr_type values;
    auto result = x3::parse(buffer.begin(), buffer.end(), lines_, values);
    assert(result);
    return values;
}

std::bitset<ast::mask_len> getTypeMask(ast::MaskBit maskBit, const ast::mask_type &mask)
{
    constexpr auto n = ast::mask_len;
    std::bitset<n> bits = 0;
    for (int i = 0; i < n; ++i) { bits[n - 1 - i] = mask[i] == maskBit; }
    return bits;
}

uint64_t f1(const ast::instr_type &data)
{
    std::unordered_map<uint64_t, uint64_t> mem;
    ast::mask_type current_mask;
    for (auto &item : data) {
        boost::apply_visitor(
            [&](const auto &var) {
                using T = std::decay_t<decltype(var)>;
                if constexpr (std::is_same_v<T, ast::mask_type>) {
                    current_mask = var;
                } else {
                    const auto [index, value] = var;
                    std::bitset<ast::mask_len> bits = value;
                    for (int i = 0; i < ast::mask_len; ++i) {
                        switch (current_mask[ast::mask_len - 1 - i]) {
                        case ast::Zero: bits[i] = false; break;
                        case ast::One: bits[i] = true; break;
                        case ast::Nothing: break;
                        }
                    }
                    mem[index] = bits.to_ullong();
                }
            },
            item);
    }
    uint64_t result = 0;
    for (const auto &[address, value] : mem) result += value;
    return result;
}

void f2_impl(std::unordered_map<uint64_t, uint64_t> &mem, const ast::mask_type &x, uint64_t value)
{
    auto pos = std::find(x.begin(), x.end(), ast::Nothing);
    if (pos == x.end()) {
        mem[getTypeMask(ast::One, x).to_ullong()] = value;
        return;
    }
    auto first = x, second = x;
    first[pos - x.begin()] = ast::Zero;
    second[pos - x.begin()] = ast::One;
    f2_impl(mem, first, value);
    f2_impl(mem, second, value);
}

uint64_t f2(const ast::instr_type &data)
{
    std::unordered_map<uint64_t, uint64_t> mem;
    ast::mask_type current_mask;
    std::vector<std::pair<ast::mask_type, uint64_t>> masks;
    for (auto &item : data) {
        boost::apply_visitor(
            [&](const auto &var) {
                using T = std::decay_t<decltype(var)>;
                if constexpr (std::is_same_v<T, ast::mask_type>) {
                    current_mask = var;
                } else {
                    const auto [index, value] = var;
                    std::bitset<ast::mask_len> bits = index;
                    auto mask = current_mask;
                    for (int i = 0; i < ast::mask_len; ++i) {
                        switch (mask[i]) {
                        case ast::Zero: mask[i] = bits[ast::mask_len - 1 - i] ? ast::One : ast::Zero; break;
                        case ast::One: bits[ast::mask_len - 1 - i] = true; break;
                        case ast::Nothing: break;
                        }
                    }
                    masks.emplace_back(mask, value);
                }
            },
            item);
    }

    for (const auto &[mask, value] : masks) { f2_impl(mem, mask, value); }

    uint64_t result = 0;
    for (const auto &[key, value] : mem) result += value;

    return result;
}

int main()
{
    const auto instr = parseFile();
    std::cout << f1(instr) << std::endl;
    std::cout << f2(instr) << std::endl;
}
