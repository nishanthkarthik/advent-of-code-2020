#include "common.h"

struct Food {
    std::set<std::string> ing;
    std::set<std::string> alg;
};

BOOST_FUSION_ADAPT_STRUCT(Food, ing, alg);

auto parseFile()
{
    using namespace boost::spirit::x3;
    auto word_ = +alpha;
    auto line_ = lexeme[word_ % " "] >> "(contains" >> (word_ % ",") >> ")";
    std::vector<Food> foods;
    const auto buffer = AOC::readFile("input.txt");
    auto result = phrase_parse(buffer.begin(), buffer.end(), +line_, space, foods);
    assert(result);
    std::cout << "Foods " << foods.size() << std::endl;
    return foods;
}

auto compute(const std::vector<Food> &foods)
{
    std::map<std::string, std::set<std::string>> map;
    for (const auto &[ing, algs] : foods) {
        for (const auto &alg : algs) {
            if (map[alg].empty()) {
                map[alg] = std::set(ing.begin(), ing.end());
            } else {
                std::set<std::string> common;
                std::set_intersection(
                    map[alg].begin(), map[alg].end(), ing.begin(), ing.end(), std::inserter(common, common.begin()));
                map[alg] = std::set(common.begin(), common.end());
            }
        }
    }

    for (const auto &[key, val]: map) {
        std::cout << key << ":\t";
        for (const auto &item: val) std::cout << item << " ";
        std::cout << std::endl;
    }

    std::set<std::string> usedIngs;
    std::map<std::string, std::string> ingToAlg;
    while (usedIngs.size() < map.size()) {
        for (const auto &[alg, ings] : map) {
            std::set<std::string> diff;
            std::set_difference(
                ings.begin(), ings.end(), usedIngs.begin(), usedIngs.end(), std::inserter(diff, diff.begin()));
            if (diff.size() == 1) {
                const auto ingNow = *diff.begin();
                ingToAlg[ingNow] = alg;
                usedIngs.insert(ingNow);
                break;
            }
        }
    }

    return ingToAlg;
}

int f1(const std::vector<Food> &foods, const std::map<std::string, std::string> &dict)
{
    std::set<std::string> allIngs;
    std::multiset<std::string> allIngsMulti;
    for (const auto &[ings, alls] : foods) {
        allIngs.insert(ings.begin(), ings.end());
        allIngsMulti.insert(ings.begin(), ings.end());
    }

    std::set<std::string> allAlgIngs;
    for (const auto &[ing, all] : dict) allAlgIngs.insert(ing);

    std::set<std::string> noAlgIngs;
    std::set_difference(allIngs.begin(), allIngs.end(), allAlgIngs.begin(), allAlgIngs.end(),
        std::inserter(noAlgIngs, noAlgIngs.begin()));

    long count = 0;
    for (const auto &ing : noAlgIngs) { count += allIngsMulti.count(ing); }
    return count;
}

std::string f2(const std::vector<Food> &foods, const std::map<std::string, std::string> &dict)
{
    std::map<std::string, std::string> revMap;
    for (const auto &[key, val]: dict) {
        revMap[val] = key;
    }
    std::ostringstream total;
    for (const auto &[key, val]: revMap) {
        total << val << ",";
    }
    auto result = total.str();
    result.pop_back();
    return result;
}

int main()
{
    const auto in = parseFile();
    const auto map = compute(in);
    std::cout << f1(in, map) << std::endl;
    std::cout << f2(in, map) << std::endl;
}