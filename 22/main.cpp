#include "common.h"
#include <numeric>

using Cards = std::deque<uint8_t>;

struct Player {
    int id;
    Cards cards;
};

std::ostream &operator<<(std::ostream &out, const Cards &cards)
{
    for (auto item : cards) std::cout << static_cast<int>(item) << " ";
    return out;
}

std::ostream &operator<<(std::ostream &out, const Player &player)
{
    std::cout << player.id << ": " << player.cards;
    return out;
}

BOOST_FUSION_ADAPT_STRUCT(Player, id, cards);

auto parseFile()
{
    using namespace boost::spirit::x3;
    auto player_header_ = "Player " >> int_ >> ":";
    auto cards_ = uint8 % eol;
    auto player_ = player_header_ >> omit[eol] >> cards_;
    auto players_ = player_ % +eol;

    const auto buffer = AOC::readFile("input.txt");

    std::vector<Player> values;
    auto res = parse(buffer.begin(), buffer.end(), players_, values);
    assert(res);

    return values;
}

int getScore(const Cards &cards)
{
    std::vector<int> factor(cards.size());
    std::iota(factor.begin(), factor.end(), 1);
    return std::inner_product(cards.begin(), cards.end(), factor.rbegin(), 0);
}

auto f1(Cards one, Cards two)
{
    while (!one.empty() && !two.empty()) {
        auto draw1 = one.front();
        auto draw2 = two.front();
        auto &winner = draw1 > draw2 ? one : two;
        winner.push_back(std::max(draw1, draw2));
        winner.push_back(std::min(draw1, draw2));
        one.pop_front();
        two.pop_front();
    }
    const auto &winner = one.empty() ? two : one;
    return winner;
}

enum class Winner {
    L,
    R,
};

std::pair<Winner, Cards> f2(Cards l, Cards r)
{
    std::set<std::pair<Cards, Cards>> cache;
    while (!l.empty() && !r.empty()) {
        if (cache.find(std::pair { l, r }) == cache.end()) {
            cache.insert(std::pair { l, r });
        } else {
            return { Winner::L, l };
        }
        const auto li = l.front();
        const auto ri = r.front();
        Winner winner;
        if (l.size() > li && r.size() > ri) {
            const auto nextL = Cards(l.begin() + 1, l.begin() + 1 + li);
            const auto nextR = Cards(r.begin() + 1, r.begin() + 1 + ri);
            winner = f2(nextL, nextR).first;
        } else {
            winner = li > ri ? Winner::L : Winner::R;
        }
        auto &wins = winner == Winner::L ? l : r;
        wins.push_back(winner == Winner::L ? li : ri);
        wins.push_back(winner == Winner::L ? ri : li);
        l.pop_front();
        r.pop_front();
    }
    return l.empty() ? std::pair { Winner::R, r } : std::pair { Winner::L, l };
}

int main()
{
    const auto in = parseFile();
    for (const auto &player : in) std::cout << player << std::endl;
    std::cout << getScore(f1(in.front().cards, in.back().cards)) << std::endl;
    std::cout << getScore(f2(in.front().cards, in.back().cards).second) << std::endl;
}