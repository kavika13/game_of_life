#ifndef GAME_OF_LIFE_CELL_HPP
#define GAME_OF_LIFE_CELL_HPP

#include <cinttypes>
#include <unordered_set>
#include <tuple>
#include <ostream>

typedef std::int64_t CellDimension;
typedef std::tuple<CellDimension, CellDimension> CellOffset;
typedef std::tuple<CellOffset, CellOffset> CellRect;

class Cell {
public:
    Cell(CellDimension x = 0, CellDimension y = 0) : x_(x), y_(y) {}

    Cell offset(const CellOffset& offset) const;
    std::size_t get_hash_code() const;

    CellDimension X() const;
    CellDimension Y() const;

private:
    friend bool operator==(const Cell& lhs, const Cell& rhs);
    friend std::ostream& operator<<(std::ostream&, const Cell&);

    CellDimension x_;
    CellDimension y_;
};

typedef std::unordered_set<Cell> CellContainer;

bool operator==(const Cell& lhs, const Cell& rhs);

namespace std {
    template <> struct hash<Cell> {
        std::size_t operator()(const Cell& cell) const {
            return cell.get_hash_code();
        }
    };
}

std::ostream& operator<<(std::ostream& os, const Cell& cell);
std::ostream& operator<<(std::ostream& os, const CellContainer& cells);
std::ostream& operator<<(std::ostream& os, const CellOffset& offset);
std::ostream& operator<<(std::ostream& os, const CellRect& rect);

CellRect get_bounds(const CellContainer& cells);

#endif // GAME_OF_LIFE_CELL_HPP
