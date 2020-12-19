#include "common.h"

#include "boost/spirit/home/x3/support/ast/variant.hpp"

using namespace boost::spirit;

namespace ast {
enum Op {
    add,
    mul,
};

struct Op_ : x3::symbols<Op> {
    Op_() { add("+", Op::add)("*", Op::mul); }
};

struct expr;
struct expr_value : x3::variant<int, x3::forward_ast<expr>> {
    using base_type::base_type;
    using base_type::operator=;
};

struct expr {
    std::vector<boost::variant<Op, expr_value>> tokens;
};
inline namespace {
    std::ostream &operator<<(std::ostream &out, const Op &);
    std::ostream &operator<<(std::ostream &out, const expr &);
    std::ostream &operator<<(std::ostream &out, const expr_value &);
    std::ostream &operator<<(std::ostream &out, const expr &val)
    {
        std::for_each(val.tokens.begin(), val.tokens.end(), [&](const auto &item) { out << item; });
        return out;
    }
    std::ostream &operator<<(std::ostream &out, const Op &op)
    {
        switch (op) {
        case Op::mul: out << '*'; break;
        case Op::add: out << '+'; break;
        }
        return out;
    }
    std::ostream &operator<<(std::ostream &out, const expr_value &val)
    {
        boost::apply_visitor(
            [&](const auto &item) {
                using T = std::decay_t<decltype(item)>;
                if constexpr (std::is_same_v<T, int>) {
                    out << item;
                } else {
                    out << '(' << item << ')';
                }
            },
            val);
        return out;
    }
}
}

BOOST_FUSION_ADAPT_STRUCT(ast::expr, tokens);

namespace parser {
using namespace boost::spirit::x3;

template <typename T> static auto as = [](auto p) { return rule<struct tag, T> { "as" } = p; };

x3::rule<struct expr_value, ast::expr_value> expr_value = "expr_value";
x3::rule<struct expr, ast::expr> expr = "expr";
const auto expr_value_def = int_ | ("(" >> expr >> ")");
const auto expr_def = as<decltype(ast::expr::tokens)>(expr_value >> *(ast::Op_() >> expr_value));

BOOST_SPIRIT_DEFINE(expr_value, expr);
}

auto parseFile()
{
    const auto buffer = AOC::readFile("input.txt");
    auto it = buffer.begin();
    auto exprs_ = parser::expr % x3::eol;
    std::vector<ast::expr> exprs;
    auto result = x3::phrase_parse(it, buffer.end(), exprs_, x3::lit(" "), exprs);
    if (buffer.end() - it > 4) std::cout << std::string(it, it + 5) << std::endl;
    assert(result);
    return exprs;
}

template <typename T> static T applyOp(ast::Op op, T l, T r)
{
    switch (op) {
    case ast::Op::mul: return l * r;
    case ast::Op::add: return l + r;
    }
    throw std::runtime_error { "NO OP" };
}

template <typename T> struct Solver1 {
    static T solve(T val) { return val; }

    static T solve(const ast::expr &expr)
    {
        T last = 0;
        ast::Op lastOp = ast::Op::add;
        for (const auto &token : expr.tokens) {
            boost::apply_visitor(
                [&](const auto &it) {
                    using M = std::decay_t<decltype(it)>;
                    if constexpr (std::is_same_v<M, ast::expr_value>) {
                        last = applyOp(lastOp, last, solve(it));
                    } else if constexpr (std::is_same_v<M, ast::Op>) {
                        lastOp = it;
                    }
                },
                token);
        }
        return last;
    }

    static T solve(const ast::expr_value &expr)
    {
        return boost::apply_visitor([](const auto &it) { return solve(it); }, expr);
    }
};

template <typename T> struct Solver2 {
    static constexpr uint8_t priority(ast::Op op)
    {
        switch (op) {
        case ast::add: return 1;
        case ast::mul: return 0;
        }
        throw std::runtime_error{"No priority"};
    }

    static T solve(const ast::expr &expr)
    {
        std::stack<ast::Op> ops;
        std::queue<boost::variant<T, ast::Op>> output;
        for (const auto &token : expr.tokens) {
            boost::apply_visitor([&](const auto &it) {
                using M = std::decay_t<decltype(it)>;
                if constexpr (std::is_same_v<M, ast::expr_value>) {
                    output.push(solve(it));
                } else if constexpr (std::is_same_v<M, ast::Op>) {
                    while (!ops.empty() && priority(ops.top()) > priority(it)) {
                        const auto opToPop = ops.top();
                        output.push(opToPop);
                        ops.pop();
                    }
                    ops.push(it);
                }
            }, token);
        }
        while (!ops.empty()) {
            output.push(ops.top());
            ops.pop();
        }
        std::stack<T> compute;
        while (!output.empty()) {
            boost::apply_visitor([&](const auto &item){
                using M = std::decay_t<decltype(item)>;
                if constexpr (std::is_same_v<M, T>) {
                    compute.push(item);
                } else if constexpr (std::is_same_v<M, ast::Op>) {
                    const T a = boost::get<T>(compute.top());
                    compute.pop();
                    const T b = boost::get<T>(compute.top());
                    compute.pop();
                    compute.push(applyOp(item, a, b));
                }
            }, output.front());
            output.pop();
        }
        return compute.top();
    }

    static T solve(const ast::expr_value &expr)
    {
        return boost::apply_visitor([](const auto &it) { return solve(it); }, expr);
    }

    static T solve(T val) { return val; }
};

int main()
{
    auto input = parseFile();
    std::cout << "Parsing done with lines: " << input.size() << std::endl;

    uint64_t total1 = 0;
    for (const auto &e : input) { total1 += Solver1<uint64_t>::solve(e); }
    std::cout << "f1 " << total1 << std::endl;

    uint64_t total2 = 0;
    for (const auto &e : input) { total2 += Solver2<uint64_t>::solve(e); }
    std::cout << "f2 " << total2 << std::endl;
}