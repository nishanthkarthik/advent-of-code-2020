#include <iostream>

#include "boost/fusion/adapted.hpp"
#include "boost/spirit/home/x3.hpp"
#include "utils.h"

enum class Instr {
    acc,
    jmp,
    nop,
};

using namespace boost::spirit;
using instr_type = std::pair<Instr, int>;

struct Instr_ : x3::symbols<Instr> {
    Instr_() { add("acc", Instr::acc)("jmp", Instr::jmp)("nop", Instr::nop); }
};

template <typename Iterator> std::vector<instr_type> parseFile(Iterator begin, Iterator end)
{
    using x3::eol;
    using x3::int_;
    using x3::space;
    auto op_ = Instr_() >> int_;
    auto ops_ = +op_;
    std::vector<instr_type> code;
    auto result = x3::phrase_parse(begin, end, ops_, (space | eol), code);
    std::cout << "Parser " << (result ? "succeeded" : "failed") << " with " << code.size() << " lines" << std::endl;
    // for (auto &line : code) std::cout << '\t' << static_cast<int>(line.first) << ' ' << line.second << std::endl;
    return code;
}

struct Machine {
    size_t pc;
    long acc;
};

Machine f1(const std::vector<instr_type> &code)
{
    std::vector<bool> executed(code.size(), false);
    Machine m { 0, 0 };
    while (!executed[m.pc] && m.pc < code.size()) {
        const auto &[instr, value] = code[m.pc];
        executed[m.pc] = true;
        switch (instr) {
        case Instr::acc:
            m.acc += value;
            ++m.pc;
            break;
        case Instr::jmp:
            m.pc += value;
            break;
        case Instr::nop:
            ++m.pc;
            break;
        }
    }
    return m;
}

Machine f2(const std::vector<instr_type> &code)
{
    for (size_t i = 0; i < code.size(); ++i) {
        if (code[i].first == Instr::acc) continue;
        auto temporary = code;
        temporary[i].first = (temporary[i].first == Instr::jmp ? Instr::nop : Instr::jmp);
        if (auto machine = f1(temporary); machine.pc == code.size()) {
            std::cout << "Found correct mutation at " << i << std::endl;
            return machine;
        }
    }
    throw std::runtime_error { "Permutation not found" };
}

int main()
{
    auto buffer = AOC::readFile("input.txt");
    auto code = parseFile(buffer.begin(), buffer.end());
    std::cout << f1(code).acc << std::endl;
    std::cout << f2(code).acc << std::endl;
}