#include "common.h"

struct Key {
    int door;
    int card;
};

BOOST_FUSION_ADAPT_STRUCT(Key, door, card);

std::ostream &operator<<(std::ostream &out, Key key)
{
    out << "Door " << key.door << ", Card " << key.card;
    return out;
}

Key parseFile()
{
    using namespace boost::spirit::x3;
    const auto buffer = AOC::readFile("input.txt");
    Key key {};
    auto result = phrase_parse(buffer.begin(), buffer.end(), int_ >> int_, eol, key);
    assert(result);
    return key;
}

inline uint64_t transform_step(uint64_t key, int subject, int divisor)
{
    key *= subject;
    key %= divisor;
    return key;
}

auto getKey(int subject, int divisor, int match)
{
    uint64_t t = 1;
    for (int i = 1; i < 1'000'000; ++i) {
        t = transform_step(t, subject, divisor);
        if (t == match) return i;
    }
    throw std::runtime_error { "No key" };
}

auto f1(Key key)
{
    constexpr auto subject = 7;
    constexpr auto divisor = 20201227;
    const auto cardLoop = getKey(subject, divisor, key.card);

    uint64_t publicKey = 1;
    for (int i = 0; i < cardLoop; ++i) { publicKey = transform_step(publicKey, key.door, divisor); }
    std::cout << publicKey << std::endl;
}

int main()
{
    const auto key = parseFile();
    std::cout << key << std::endl;
    f1(key);
}