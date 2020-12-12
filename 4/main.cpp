#include <algorithm>
#include <array>
#include <cctype>
#include <fstream>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "boost/fusion/adapted.hpp"
#include "boost/fusion/tuple.hpp"
#include "boost/spirit/home/x3.hpp"
#include "boost/variant.hpp"

using namespace boost::spirit;

enum class FieldType {
    byr,
    iyr,
    eyr,
    hgt,
    hcl,
    ecl,
    pid,
    cid,
};

enum class LengthUnit { in, cm };

using ColorHex = std::array<uint, 3>;

std::vector<char> readFile()
{
    std::ifstream in { "input.txt", std::ios::ate };
    size_t length = in.tellg();
    in.seekg(std::ios::beg);
    std::vector<char> result(length);
    in.read(result.data(), length);
    return result;
}

struct FieldType_ : x3::symbols<FieldType> {
    FieldType_()
    {
        add("byr", FieldType::byr)("iyr", FieldType::iyr)("eyr", FieldType::eyr)("hgt", FieldType::hgt)(
            "hcl", FieldType::hcl)("ecl", FieldType::ecl)("pid", FieldType::pid)("cid", FieldType::cid);
    }
};

struct LengthUnitType_ : x3::symbols<LengthUnit> {
    LengthUnitType_() { add("cm", LengthUnit::cm)("in", LengthUnit::in); }
};

enum class ColorPalette {
    amb,
    blu,
    brn,
    gry,
    grn,
    hzl,
    oth,
};

struct ColorPalette_ : x3::symbols<ColorPalette> {
    ColorPalette_()
    {
        add("amb", ColorPalette::amb)("blu", ColorPalette::blu)("brn", ColorPalette::brn)("gry", ColorPalette::gry)(
            "grn", ColorPalette::grn)("hzl", ColorPalette::hzl)("oth", ColorPalette::oth);
    }
};

using fixedLengthID_type = std::vector<char>;
using numeric_type = unsigned long;
using length_type = boost::tuple<numeric_type, LengthUnit>;
using colorHex_type = std::vector<uint8_t>;
using colorPalette_type = ColorPalette;
using value_type
    = boost::variant<colorHex_type, colorPalette_type, length_type, fixedLengthID_type, numeric_type, std::string>;

auto parseFile(const std::vector<char> &v)
{
    using x3::alnum;
    using x3::char_;
    using x3::digit;
    using x3::double_;
    using x3::eol;
    using x3::graph;
    using x3::omit;
    using hex = x3::uint_parser<uint8_t, 16, 2, 2>;
    using x3::phrase_parse;
    using x3::repeat;
    using x3::ulong_;
    using x3::ascii::space;

    auto key = FieldType_();

    auto id = ulong_;

    struct FixedLengthIDTag;
    auto fixedLengthID = x3::rule<FixedLengthIDTag, fixedLengthID_type>() = repeat(9)[digit];

    struct Length_tag;
    auto length = x3::rule<Length_tag, length_type>() = (ulong_ >> LengthUnitType_());

    struct ColorHex_tag;
    auto colorHex = x3::rule<ColorHex_tag, colorHex_type>() = ('#' >> repeat(3)[hex()]);

    struct ColorPalette_tag;
    auto colorPalette = x3::rule<ColorPalette_tag, colorPalette_type>() = ColorPalette_();

    struct Value_tag;
    auto field_sep = (space | eol);
    auto value = x3::rule<Value_tag, value_type>()
        = (colorHex | colorPalette | length | (fixedLengthID >> &field_sep) | (id >> &field_sep) | +graph);

    struct Field_tag;
    using keyValue_type = boost::tuple<FieldType, value_type>;
    auto keyValue = x3::rule<Field_tag, keyValue_type>() = (key >> ':' >> value);

    using passport_type = std::map<FieldType, value_type>;
    auto passport = keyValue % (space | eol);

    using passports_type = std::vector<passport_type>;
    auto passports = passport % +eol;

    passports_type val;
    auto it = v.begin();
    auto result = parse(it, v.end(), passports, val);
    std::cout << "Parser " << (result ? "succeeded" : "failed") << std::endl;

    std::cout << "Passports read: " << val.size() << std::endl;
    if (v.end() - it > 10) {
        throw std::runtime_error { "Parse failed at " + std::to_string(it - v.begin()) + " / "
            + std::to_string(v.size()) + " where " + std::string(it, it + 10) };
    }
    return val;
}

const auto passportPreliminaryCheck = [](const auto &passport) -> bool {
    return passport.size() == 8 || (passport.size() == 7 && passport.find(FieldType::cid) == passport.end());
};

template <typename T> int f1(const T &passports)
{
    int count = 0;
    for (const auto &passport : passports) {
        count += passportPreliminaryCheck(passport);
    }
    return count;
}

template <typename T> size_t f2(const std::vector<std::map<FieldType, T>> &passports)
{
    std::vector<bool> isPassportValid(passports.size());
    std::transform(
        passports.begin(), passports.end(), isPassportValid.begin(), [](const std::map<FieldType, value_type> &item) {
            if (!passportPreliminaryCheck(item)) return false;

            enum { ColorHexPos, ColorPalPos, LengthPos, FixedLengthIDPos, NumPos, NonePos };

            // byr
            if (item.at(FieldType::byr).which() != NumPos) return false;
            auto byr = boost::get<numeric_type>(item.at(FieldType::byr));
            if (byr < 1920 || byr > 2002) return false;

            // iyr
            if (item.at(FieldType::iyr).which() != NumPos) return false;
            auto iyr = boost::get<numeric_type>(item.at(FieldType::iyr));
            if (iyr < 2010 || iyr > 2020) return false;

            // eyr
            if (item.at(FieldType::eyr).which() != NumPos) return false;
            auto eyr = boost::get<numeric_type>(item.at(FieldType::eyr));
            if (eyr < 2020 || eyr > 2030) return false;

            // hgt
            if (item.at(FieldType::hgt).which() != LengthPos) return false;
            auto hgt = boost::get<length_type>(item.at(FieldType::hgt));
            auto hgtValue = hgt.get<0>();
            auto hgtUnit = hgt.get<1>();
            if ((hgtUnit == LengthUnit::cm && (hgtValue < 150 || hgtValue > 193))
                || (hgtUnit == LengthUnit::in && (hgtValue < 59 || hgtValue > 76)))
                return false;

            // hcl
            if (item.at(FieldType::hcl).which() != ColorHexPos) return false;

            // ecl
            if (item.at(FieldType::ecl).which() != ColorPalPos) return false;

            // pid
            if (item.at(FieldType::pid).which() != FixedLengthIDPos) return false;

            return true;
        });
    return std::accumulate(isPassportValid.begin(), isPassportValid.end(), 0);
}

int main()
{
    const auto contents = readFile();
    const auto result = parseFile(contents);
    std::cout << f1(result) << std::endl;
    std::cout << f2(result) << std::endl;
}