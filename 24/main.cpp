#include "boost/multi_array.hpp"
#include "common.h"

#include <list>

enum class Dir : uint8_t {
    E,
    NE,
    NW,
    W,
    SW,
    SE,
};

std::array<Dir, 6> Dirs = { Dir::E, Dir::NE, Dir::NW, Dir::W, Dir::SW, Dir::SE };

Dir opposite(Dir dir) { return Dirs[(static_cast<uint8_t>(dir) + 3) % Dirs.size()]; }

std::pair<Dir, Dir> offByTwo(Dir dir)
{
    return std::pair {
        Dirs[(static_cast<uint8_t>(dir) + Dirs.size() + 2) % Dirs.size()],
        Dirs[(static_cast<uint8_t>(dir) + Dirs.size() - 2) % Dirs.size()],
    };
}

Dir mid(Dir one, Dir two)
{
    static std::map<std::set<Dir>, Dir> map {
        { { Dir::E, Dir::NW }, Dir::NE },
        { { Dir::NE, Dir::W }, Dir::NW },
        { { Dir::NW, Dir::SW }, Dir::W },
        { { Dir::W, Dir::SE }, Dir::SW },
        { { Dir::SW, Dir::E }, Dir::SE },
        { { Dir::SE, Dir::NE }, Dir::E },
    };
    return map[{ one, two }];
}

struct Dir_ : boost::spirit::x3::symbols<Dir> {
    Dir_() { add("ne", Dir::NE)("se", Dir::SE)("nw", Dir::NW)("sw", Dir::SW)("e", Dir::E)("w", Dir::W); }
};

using Tile = std::vector<Dir>;
using Tiles = std::vector<Tile>;

std::ostream &operator<<(std::ostream &out, Dir dir)
{
    switch (dir) {
    case Dir::E: out << "E"; break;
    case Dir::NE: out << "NE"; break;
    case Dir::NW: out << "NW"; break;
    case Dir::W: out << "W"; break;
    case Dir::SW: out << "SW"; break;
    case Dir::SE: out << "SE"; break;
    }
    return out;
}

std::ostream &operator<<(std::ostream &out, const std::list<Dir> &tile)
{
    for (auto dir : tile) { std::cout << dir << ' '; }
    return out;
}

std::ostream &operator<<(std::ostream &out, const Tile &tile)
{
    for (auto dir : tile) { std::cout << dir << ' '; }
    return out;
}

auto parseFile()
{
    using namespace boost::spirit::x3;
    auto tile_ = +Dir_();
    auto tiles_ = tile_ % eol;
    Tiles tiles;
    const auto buffer = AOC::readFile("input.txt");
    auto res = parse(buffer.begin(), buffer.end(), tiles_, tiles);
    assert(res);
    return tiles;
}

Tile simplify(std::list<Dir> tile)
{
    int size;
    do {
        size = tile.size();
        for (auto it = tile.begin(); it != tile.end() && std::next(it) != tile.end(); ++it) {
            auto [adj1, adj2] = offByTwo(*it);
            auto match = std::optional<Dir>();
            if (*std::next(it) == adj1) {
                match = adj1;
            } else if (*std::next(it) == adj2) {
                match = adj2;
            }

            if (match.has_value()) {
                auto toAdd = mid(*match, *it);
                tile.insert(it, toAdd);
                auto toRemove1 = it;
                auto toRemove2 = std::next(it);
                std::advance(it, 2);
                tile.erase(toRemove1);
                tile.erase(toRemove2);
            }
        }

        for (auto it = tile.begin(); it != tile.end() && std::next(it) != tile.end(); ++it) {
            if (*std::next(it) == opposite(*it)) {
                auto toRemove1 = it;
                auto toRemove2 = std::next(it);
                std::advance(it, 2);
                tile.erase(toRemove1);
                tile.erase(toRemove2);
            }
        }
    } while (size != tile.size());
    return { tile.begin(), tile.end() };
}

using Grid = boost::multi_array<bool, 2>;
using extents = Grid::extent_gen;
using range = Grid::extent_range;
constexpr auto DIM = 200;

inline std::pair<int, int> offset(Dir dir)
{
    switch (dir) {
    case Dir::E: return std::pair { 0, 2 };
    case Dir::W: return std::pair { 0, -2 };
    case Dir::NE: return std::pair { -1, 1 };
    case Dir::SE: return std::pair { 1, 1 };
    case Dir::NW: return std::pair { -1, -1 };
    case Dir::SW: return std::pair { 1, -1 };
    }
    throw std::runtime_error { "No offset for dir" };
}

inline std::pair<int, int> move(std::pair<int, int> from, Dir dir)
{
    auto [offX, offY] = offset(dir);
    return std::pair { from.first + offX, from.second + offY };
}

inline auto makeGrid()
{
    auto extent = extents();
    auto grid = Grid(extent[range(-DIM, DIM + 1)][range(-DIM, DIM + 1)]);
    return grid;
}

auto countTotalBlackTiles(const Grid &grid)
{
    long total = 0;
    for (int i = -DIM; i <= DIM; ++i) {
        for (int j = -DIM; j <= DIM; ++j) { total += grid[i][j]; }
    }
    return total;
}

auto f1(const Tiles &tiles)
{
    auto grid = makeGrid();
    for (const auto &tile : tiles) {
        std::pair now { 0, 0 };
        for (const auto &dir : tile) { now = move(now, dir); }
        grid[now.first][now.second] ^= true;
    }
    std::cout << "Total black tiles " << countTotalBlackTiles(grid) << std::endl;
    return grid;
}

void inplace_simplify(Tiles &tiles)
{
    for (auto &tile : tiles) {
        auto simplified = simplify({ tile.begin(), tile.end() });
        tile = simplified;
    }
}

int blackTileNeighborCount(const Grid &grid, std::pair<int, int> at)
{
    int count = 0;
    for (auto dir : Dirs) {
        auto [nX, nY] = move(at, dir);
        count += grid[nX][nY];
    }
    return count;
}

Grid f2_pass(const Grid &from)
{
    Grid to = from;
    constexpr auto safeZoneOffset = 2;
    for (int i = -DIM + safeZoneOffset; i <= DIM - safeZoneOffset; ++i) {
        for (int j = -DIM + safeZoneOffset; j <= DIM - safeZoneOffset; ++j) {
            if ((std::abs(i) + std::abs(j)) % 2 != 0) continue;
            const auto blackTileNeighbors = blackTileNeighborCount(from, { i, j });
            if (from[i][j]) { // black
                if (blackTileNeighbors == 0 || blackTileNeighbors > 2) to[i][j] = false;
            } else { // white
                if (blackTileNeighbors == 2) to[i][j] = true;
            }
        }
    }
    return to;
}

void f2(const Tiles &tiles)
{
    auto now = f1(tiles);
    constexpr auto iterations = 101;
    for (int i = 0; i < iterations; ++i) {
        std::cout << "Day " << i << ": " << countTotalBlackTiles(now) << std::endl;
        now = f2_pass(now);
    }
}

int main()
{
    auto in = parseFile();
    inplace_simplify(in);
    f1(in);
    f2(in);
}