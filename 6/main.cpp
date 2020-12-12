#include <fstream>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <vector>

std::vector<std::vector<std::string>> readFile()
{
    std::ifstream in { "input.txt" };
    std::vector<std::vector<std::string>> result;
    while (in.good()) {
        std::vector<std::string> group;
        std::string string;
        while (std::getline(in, string), !string.empty()) {
            group.emplace_back(string);
            string.clear();
        }
        result.emplace_back(group);
    }
    return result;
}

size_t f1(const std::vector<std::vector<std::string>> &groups)
{
    size_t result = 0;
    for (const auto &group : groups) {
        std::unordered_set<char> set;
        for (const auto &answer : group) {
            for (auto item : answer) set.insert(item);
        }
        result += set.size();
    }
    return result;
}

size_t f2(const std::vector<std::vector<std::string>> &groups)
{
    size_t result = 0;
    for (const auto &group : groups) {
        std::unordered_map<char, size_t> map;
        for (const auto &answer : group) {
            for (auto item : answer) ++map[item];
        }

        size_t allAnswered = 0;
        for (char i = 'a'; i <= 'z'; ++i) {
            allAnswered += (map[i] == group.size());
        }

        result += allAnswered;
    }
    return result;
}

int main()
{
    auto input = readFile();
    std::cout << f1(input) << std::endl;
    std::cout << f2(input) << std::endl;
}