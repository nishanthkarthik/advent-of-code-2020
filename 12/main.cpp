#include <complex>
#include <iostream>

#include "boost/fusion/adapted.hpp"
#include "boost/spirit/home/x3.hpp"

#include "utils.h"

enum class Op {
    N,
    S,
    E,
    W,
    L,
    R,
    F,
};

struct Op_ : boost::spirit::x3::symbols<Op> {
    Op_() { add("N", Op::N)("S", Op::S)("E", Op::E)("W", Op::W)("L", Op::L)("R", Op::R)("F", Op::F); }
};

std::vector<std::pair<Op, int>> parseFile()
{
    using namespace boost::spirit::x3;
    const auto buffer = AOC::readFile("input.txt");
    std::vector<std::pair<Op, int>> values;
    auto row_ = Op_() >> int_;
    auto result = parse(buffer.begin(), buffer.end(), row_ % eol, values);
    assert(result);
    std::cout << "Read " << values.size() << " entries" << std::endl;
    return values;
}

struct Ferry {
    int headingDegrees;
    double x, y;
};

double toRadian(double value)
{
    const double pi = std::acos(-1);
    return pi * value / 180.0;
};

double f1(const std::vector<std::pair<Op, int>> &data)
{
    Ferry ferry {};

    for (auto [op, value] : data) {
        switch (op) {
        case Op::N: ferry.y += value; break;
        case Op::S: ferry.y -= value; break;
        case Op::E: ferry.x += value; break;
        case Op::W: ferry.x -= value; break;
        case Op::L:
            ferry.headingDegrees += value;
            ferry.headingDegrees %= 360;
            break;
        case Op::R:
            ferry.headingDegrees += 360 - value;
            ferry.headingDegrees %= 360;
            break;
        case Op::F:
            ferry.x += value * std::cos(toRadian(ferry.headingDegrees));
            ferry.y += value * std::sin(toRadian(ferry.headingDegrees));
            break;
        }
    }
    return std::abs(ferry.x) + std::abs(ferry.y);
}

double f2(const std::vector<std::pair<Op, int>> &data)
{
    std::complex<double> ferry { 0, 0 };
    std::complex<double> point { 10, 1 };

    for (auto [op, value] : data) {
        switch (op) {
        case Op::N: {
            auto offset = std::complex<double> { 0, static_cast<double>(value) };
            point += offset;
            break;
        }
        case Op::S: {
            auto offset = std::complex<double> { 0, -static_cast<double>(value) };
            point += offset;
            break;
        }
        case Op::E: {
            auto offset = std::complex<double> { static_cast<double>(value), 0 };
            point += offset;
            break;
        }
        case Op::W: {
            auto offset = std::complex<double> { -static_cast<double>(value), 0 };
            point += offset;
            break;
        }
        case Op::L: {
            auto temp = point - ferry;
            auto radian = toRadian(value);
            temp *= std::complex { std::cos(radian), std::sin(radian) };
            point = temp + ferry;
            break;
        }
        case Op::R: {
            auto temp = point - ferry;
            auto radian = toRadian(360 - value);
            temp *= std::complex { std::cos(radian), std::sin(radian) };
            point = temp + ferry;
            break;
        }
        case Op::F: {
            auto diff = point - ferry;
            ferry += diff * static_cast<double>(value);
            point += diff * static_cast<double>(value);
            break;
        }
        }
    }
    return std::abs(ferry.real()) + std::abs(ferry.imag());
}

int main()
{
    const auto data = parseFile();
    std::cout << f1(data) << std::endl;
    std::cout << f2(data) << std::endl;
}