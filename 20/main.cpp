#include "common.h"
#include <fstream>

constexpr auto DIM = 10;

struct Tile1 {
    std::bitset<DIM> t, b, l, r;
};

struct Bin_ : boost::spirit::x3::symbols<bool> {
    Bin_() { add(".", false)("#", true); }
};

using PreData = std::vector<std::vector<bool>>;
using PreTile = std::unordered_map<int, PreData>;
using PreResult = std::unordered_map<int, Tile1>;

auto parseFile()
{
    using namespace boost::spirit::x3;
    auto header_ = "Tile " >> int_ >> ":";
    auto tilerow_ = repeat(DIM)[Bin_()];
    auto tiledata_ = tilerow_ % eol;
    auto tile_ = header_ >> eol >> tiledata_;
    PreTile t;
    auto buffer = AOC::readFile("input.txt");
    auto res = parse(buffer.begin(), buffer.end(), tile_ % +eol, t);
    assert(res);
    std::cout << "read tiles " << t.size() << std::endl;
    return t;
}

PreResult preprocess1(const PreTile &preTile)
{
    std::unordered_map<int, Tile1> tiles;
    for (const auto &[idx, data] : preTile) {
        Tile1 tile {};
        tile.t = tile.b = tile.l = tile.r = 0;
        for (int i = 0; i < DIM; ++i) {
            tile.t[i] = data[0][i];
            tile.b[i] = data[DIM - 1][i];
            tile.l[i] = data[i][0];
            tile.r[i] = data[i][DIM - 1];
        }
        tiles[idx] = tile;
    }
    // for (const auto &[idx, tile] : tiles) {
    //     std::cout << idx << '\t';
    //     std::cout << tile.l.to_ulong() << ' ';
    //     std::cout << tile.t.to_ulong() << ' ';
    //     std::cout << tile.r.to_ulong() << ' ';
    //     std::cout << tile.b.to_ulong() << std::endl;
    // }
    return tiles;
}

template <size_t N> constexpr std::bitset<N> reverse(std::bitset<N> x)
{
    std::bitset<N> r;
    for (int i = 0; i < N; ++i) { r[N - i - 1] = x[i]; }
    return r;
}

void f1(const std::unordered_map<int, Tile1> &tiles)
{
    std::unordered_map<uint64_t, std::vector<int>> counter;
    for (const auto &[idx, tile] : tiles) {
        counter[std::min(reverse(tile.t).to_ulong(), tile.t.to_ulong())].emplace_back(idx);
        counter[std::min(reverse(tile.b).to_ulong(), tile.b.to_ulong())].emplace_back(idx);
        counter[std::min(reverse(tile.l).to_ulong(), tile.l.to_ulong())].emplace_back(idx);
        counter[std::min(reverse(tile.r).to_ulong(), tile.r.to_ulong())].emplace_back(idx);
    }
    // tile idx -> one match count
    std::unordered_map<int, int> oneMatchCount;
    for (const auto &[sign, indices] : counter) {
        if (indices.size() == 1) ++oneMatchCount[indices.front()];
    }

    std::cout << "Corners\t";
    uint64_t product = 1;
    for (auto [key, val] : oneMatchCount) {
        // tile with two unmatched sides is a corner
        if (val == 2) {
            product *= key;
            std::cout << key << ' ';
        }
    }
    std::cout << std::endl;
    std::cout << "Product " << product << std::endl;
}

struct Tile2 {
    std::bitset<DIM> t, b, l, r;
};

using Graph = std::unordered_map<int, std::set<int>>;
using Grid = std::vector<std::vector<int>>;
using Picture = std::vector<std::vector<bool>>;

Graph f2_graph(const std::unordered_map<int, Tile1> &input)
{
    std::unordered_map<int, Tile2> tiles;
    for (const auto &[idx, tile] : input) {
        using C = decltype(Tile2::t);
        tiles[idx] = Tile2 {
            C { std::min(tile.t.to_ulong(), reverse(tile.t).to_ulong()) },
            C { std::min(tile.b.to_ulong(), reverse(tile.b).to_ulong()) },
            C { std::min(tile.l.to_ulong(), reverse(tile.l).to_ulong()) },
            C { std::min(tile.r.to_ulong(), reverse(tile.r).to_ulong()) },
        };
    }

    std::unordered_map<int, std::vector<int>> indices;
    for (const auto &[idx, tile] : tiles) {
        indices[tile.t.to_ulong()].emplace_back(idx);
        indices[tile.b.to_ulong()].emplace_back(idx);
        indices[tile.l.to_ulong()].emplace_back(idx);
        indices[tile.r.to_ulong()].emplace_back(idx);
    }

    std::unordered_map<int, std::set<int>> graph;
    std::unordered_map<int, bool> visited;
    std::queue<int> bfs;
    bfs.push(input.begin()->first);
    while (!bfs.empty()) {
        const auto node = bfs.front();
        visited[node] = true;
        bfs.pop();
        const auto tile = tiles[node];
        for (auto sign : { tile.l, tile.r, tile.t, tile.b }) {
            const auto &match = indices[sign.to_ulong()];
            if (match.size() == 2) {
                for (auto e : match) {
                    if (e == node) continue;
                    if (!visited[e]) bfs.push(e);
                    graph[node].insert(e);
                }
            }
        }
    }

    //    for (const auto &[node, to_nodes] : graph) {
    //        std::cout << node << '\t';
    //        for (auto e : to_nodes) std::cout << e << ' ';
    //        std::cout << std::endl;
    //    }
    return graph;
}

enum class Dir {
    left,
    top,
    right,
    bottom,
};

Grid f2_reconstruct(const Graph &graph)
{
    const int N = std::sqrt(graph.size());
    constexpr auto NONE = -1;
    std::vector<std::vector<int>> grid(N, std::vector<int>(N, NONE));
    const auto [topLeft, tlNeigh]
        = *std::find_if(graph.begin(), graph.end(), [](const auto &el) { return el.second.size() == 2; });
    std::cout << "Top left as " << topLeft << std::endl;

    grid[0][0] = topLeft;
    grid[0][1] = *tlNeigh.begin();
    grid[1][0] = *tlNeigh.rbegin();

    const auto printGrid = [&] {
        for (const auto &row : grid) {
            for (const auto &e : row) {
                if (e != NONE)
                    std::cout << e;
                else
                    std::cout << "____";
                std::cout << "\t";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    };

    const auto isGridNotFull = [&] {
        for (const auto &row : grid) {
            for (const auto &item : row) {
                if (item == NONE) return true;
            }
        }
        return false;
    };

    const auto totalNeighbors = [N](int i, int j) {
        std::set<Dir> set;
        if (i != 0) set.insert(Dir::top);
        if (j != 0) set.insert(Dir::left);
        if (i != N - 1) set.insert(Dir::bottom);
        if (j != N - 1) set.insert(Dir::right);
        return set;
    };

    const auto filledNeighbors = [&](int i, int j) {
        std::set<std::pair<int, Dir>> neigh;
        if (i > 0 && grid[i - 1][j] != NONE) neigh.insert(std::pair { grid[i - 1][j], Dir::top });
        if (j > 0 && grid[i][j - 1] != NONE) neigh.insert(std::pair { grid[i][j - 1], Dir::left });
        if (i < (N - 1) && grid[i + 1][j] != NONE) neigh.insert(std::pair { grid[i + 1][j], Dir::bottom });
        if (j < (N - 1) && grid[i][j + 1] != NONE) neigh.insert(std::pair { grid[i][j + 1], Dir::right });
        return neigh;
    };

    const auto dirOffset = [](Dir d) {
        switch (d) {
        case Dir::left: return std::make_pair(0, -1);
        case Dir::top: return std::make_pair(-1, 0);
        case Dir::right: return std::make_pair(0, 1);
        case Dir::bottom: return std::make_pair(1, 0);
        }
        throw std::runtime_error { "ENUM Dir" };
    };

    while (isGridNotFull()) {
        // Fill neighbors
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                const auto now = grid[i][j];
                if (now == NONE) continue;
                const auto TN = totalNeighbors(i, j);
                const auto FN = filledNeighbors(i, j);
                if (TN.size() == FN.size()) continue;
                if (TN.size() - FN.size() > 1) continue;
                const auto &graphNeighbors = graph.at(now);

                std::set<int> filledIdx;
                std::set<Dir> filledPos;
                for (auto [key, dir] : FN) {
                    filledIdx.insert(key);
                    filledPos.insert(dir);
                }

                std::set<int> toFill;
                std::set_difference(graphNeighbors.begin(), graphNeighbors.end(), filledIdx.begin(), filledIdx.end(),
                    std::inserter(toFill, toFill.begin()));
                assert(toFill.size() == 1);

                std::set<Dir> toFillPos;
                std::set_difference(TN.begin(), TN.end(), filledPos.begin(), filledPos.end(),
                    std::inserter(toFillPos, toFillPos.begin()));
                assert(toFillPos.size() == 1);

                const auto [xOff, yOff] = dirOffset(*toFillPos.begin());
                grid[i + xOff][j + yOff] = *toFill.begin();
            }
        }

        // Fill empty spots
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                // should not be any corners or edges
                const auto TN = totalNeighbors(i, j);
                if (TN.size() != 4) continue;
                const auto FN = filledNeighbors(i, j);
                std::set<int> filledIdx;
                std::set<Dir> filledPos;
                for (auto [key, dir] : FN) {
                    filledIdx.insert(key);
                    filledPos.insert(dir);
                }

                constexpr std::array<std::pair<Dir, Dir>, 4> diags {
                    std::pair { Dir::top, Dir::left },
                    std::pair { Dir::top, Dir::right },
                    std::pair { Dir::bottom, Dir::left },
                    std::pair { Dir::bottom, Dir::right },
                };
                for (auto [m, n] : diags) {
                    if (filledPos.find(m) != filledPos.end() && filledPos.find(n) != filledPos.end()) {
                        const auto [mxOff, myOff] = dirOffset(m);
                        const auto [nxOff, nyOff] = dirOffset(n);
                        const auto M_Idx = grid[i + mxOff][j + myOff];
                        const auto N_Idx = grid[i + nxOff][j + nyOff];
                        assert(M_Idx != NONE && N_Idx != NONE);
                        const auto &M_set = graph.at(M_Idx);
                        const auto &N_set = graph.at(N_Idx);
                        std::vector<int> diff;
                        std::set_intersection(
                            M_set.begin(), M_set.end(), N_set.begin(), N_set.end(), std::inserter(diff, diff.begin()));
                        assert(diff.size() == 2);
                        const auto diag = grid[i + mxOff + nxOff][j + myOff + nyOff];
                        grid[i][j] = diff[0] == diag ? diff[1] : diff[0];
                        break;
                    }
                }
            }
        }
    }

    std::cout << "Reconstructed image" << std::endl;
    printGrid();
    return grid;
}

PreData rotateAntiClk(PreData in)
{
    for (int i = 0; i < DIM / 2; ++i) {
        for (int j = i; j < DIM - i - 1; ++j) {
            const bool tmp = in[i][j];
            in[i][j] = in[j][DIM - 1 - i];
            in[j][DIM - 1 - i] = in[DIM - 1 - i][DIM - 1 - j];
            in[DIM - 1 - i][DIM - 1 - j] = in[DIM - 1 - j][i];
            in[DIM - 1 - j][i] = tmp;
        }
    }
    return in;
}

PreData flipV(PreData in)
{
    for (int i = 0; i < DIM / 2; ++i) { std::swap(in[i], in[DIM - 1 - i]); }
    return in;
}

PreData flipH(PreData in)
{
    for (int i = 0; i < DIM; ++i) {
        for (int j = 0; j < DIM / 2; ++j) { std::swap(in[i][j], in[i][DIM - 1 - j]); }
    }
    return in;
}

size_t edge(const PreData &data, Dir dir)
{
    std::bitset<DIM> bits;
    switch (dir) {
    case Dir::left: {
        for (int i = 0; i < DIM; ++i) bits[i] = data[i][0];
        break;
    }
    case Dir::right: {
        for (int i = 0; i < DIM; ++i) bits[i] = data[i][DIM - 1];
        break;
    }
    case Dir::top: {
        for (int i = 0; i < DIM; ++i) bits[i] = data[0][i];
        break;
    }
    case Dir::bottom: {
        for (int i = 0; i < DIM; ++i) bits[i] = data[DIM - 1][i];
        break;
    }
    }
    return bits.to_ulong();
}

void drawAt(Picture &pic, const PreData &data, int ii, int jj)
{
    for (int i = DIM * ii; i < DIM * ii + DIM; ++i) {
        for (int j = DIM * jj; j < DIM * jj + DIM; ++j) { pic[i][j] = data[i - DIM * ii][j - DIM * jj]; }
    }
}

void drawPicture(const Picture &pic)
{
    const int N = pic.size();
    for (int i = 0; i < N; ++i) {
        if (i % 10 == 0) std::cout << std::endl;
        for (int j = 0; j < N; ++j) {
            if (j % 10 == 0) std::cout << '|';
            std::cout << (pic[i][j] ? '#' : '.');
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

PreData tileFromPicture(const Picture &pic, int ii, int jj)
{
    PreData data(DIM, std::vector<bool>(DIM, false));
    for (int i = DIM * ii; i < DIM * ii + DIM; ++i) {
        for (int j = DIM * jj; j < DIM * jj + DIM; ++j) { data[i - DIM * ii][j - DIM * jj] = pic[i][j]; }
    }
    return data;
}

Picture f2_draw(const Grid &grid, const PreResult &pre, PreTile input)
{
    const int N = std::sqrt(pre.size());
    Picture pic(DIM * N, std::vector<bool>(DIM * N, false));

    // Bad bad hacky hack!
    drawAt(pic, flipV(input[grid[0][0]]), 0, 0);

    std::cout << "H sweep" << std::endl;
    // horizontal sweep
    for (int j = 1; j < N; ++j) {
        const auto nowIdx = grid[0][j];
        auto now = input[nowIdx];
        const auto left = tileFromPicture(pic, 0, j - 1);
        const auto reference = edge(left, Dir::right);
        while (true) {
            if (edge(now, Dir::left) == reference || edge(flipV(now), Dir::left) == reference) break;
            now = rotateAntiClk(now);
        }
        if (reference != edge(now, Dir::left)) now = flipV(now);
        drawAt(pic, now, 0, j);
    }

    std::cout << "V sweep" << std::endl;
    // vertical sweep
    for (int i = 1; i < N; ++i) {
        const auto nowIdx = grid[i][0];
        auto now = input[nowIdx];
        const auto top = tileFromPicture(pic, i - 1, 0);
        const auto reference = edge(top, Dir::bottom);
        while (true) {
            if (edge(now, Dir::top) == reference || edge(flipH(now), Dir::top) == reference) break;
            now = rotateAntiClk(now);
        }
        if (reference != edge(now, Dir::top)) now = flipH(now);
        drawAt(pic, now, i, 0);
    }

    std::cout << "T sweep" << std::endl;
    // top-reference sweep
    for (int i = 1; i < N; ++i) {
        for (int j = 1; j < N; ++j) {
            const auto nowIdx = grid[i][j];
            auto now = input[nowIdx];
            const auto top = tileFromPicture(pic, i - 1, j);
            const auto reference = edge(top, Dir::bottom);
            while (true) {
                if (edge(now, Dir::top) == reference || edge(flipH(now), Dir::top) == reference) break;
                now = rotateAntiClk(now);
            }
            if (reference != edge(now, Dir::top)) now = flipH(now);

            drawAt(pic, now, i, j);
        }
    }

    return pic;
}

Picture f2_remove_border(const Picture &in)
{
    const auto N = in.size();
    Picture out;
    for (int i = 0; i < N; ++i) {
        if (i % DIM == 0 || i % DIM == (DIM - 1)) continue;
        std::vector<bool> t;
        for (int j = 0; j < N; ++j) {
            if (j % DIM == 0 || j % DIM == (DIM - 1)) continue;
            t.emplace_back(in[i][j]);
        }
        out.emplace_back(t);
    }
    std::cout << out.size() << ", " << out.front().size() << std::endl;
    return out;
}

int main()
{
    const auto input = parseFile();
    const auto pre1 = preprocess1(input);
    f1(pre1);
    const auto grid = f2_reconstruct(f2_graph(pre1));
    const auto drawing = f2_draw(grid, pre1, input);
    const auto image = f2_remove_border(drawing);
    std::ofstream out("image.txt", std::ios::trunc);
    for (const auto &row : image) {
        for (const auto &col : row) {
            out << (col ? 1 : 0) << ",";
        }
        out << std::endl;
    }
}