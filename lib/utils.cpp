#include "utils.h"

#include <fstream>

namespace AOC {
std::vector<char> readFile(const std::string &file)
{
    std::ifstream in { file, std::ios::ate };
    std::vector<char> buffer(in.tellg());
    in.seekg(std::ios::beg);
    in.read(buffer.data(), buffer.size());
    return buffer;
}
}
