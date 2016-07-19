#include <algorithm>
#include "cell.hpp"

Cell Cell::offset(const CellOffset& offset) const {
    return Cell(x_ + std::get<0>(offset), y_ + std::get<1>(offset));
}

std::size_t Cell::get_hash_code() const {
    std::size_t result = 17;
    result = result * 23 + std::hash<CellDimension>()(x_);
    result = result * 23 + std::hash<CellDimension>()(y_);
    return result;
}

CellDimension Cell::X() const {
    return x_;
}

CellDimension Cell::Y() const {
    return y_;
}

bool operator==(const Cell& lhs, const Cell& rhs) {
    return lhs.x_ == rhs.x_ && lhs.y_ == rhs.y_;
}

std::ostream& operator<<(std::ostream& os, const Cell& cell) {
   return os << "<Cell: " << cell.x_ << ", " << cell.y_ << ">";
}

std::ostream& operator<<(std::ostream& os, const CellContainer& cells) {
    for(const Cell& cell: cells) {
        os << cell << ", ";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const CellOffset& offset) {
    return os << "<" << std::get<0>(offset) << ", " << std::get<1>(offset) << ">";
}

std::ostream& operator<<(std::ostream& os, const CellRect& rect) {
    return os << "<" << std::get<0>(rect) << ", " << std::get<1>(rect) << ">";
}

CellRect get_bounds(const CellContainer& cells) {
    CellOffset lower_bound, upper_bound;

    for(const Cell& cell: cells) {
        std::get<0>(lower_bound) = std::min(cell.X(), std::get<0>(lower_bound));
        std::get<1>(lower_bound) = std::min(cell.Y(), std::get<1>(lower_bound));
        std::get<0>(upper_bound) = std::max(cell.X(), std::get<0>(upper_bound));
        std::get<1>(upper_bound) = std::max(cell.Y(), std::get<1>(upper_bound));
    }

    return CellRect(
        lower_bound,
        CellOffset(
            std::get<0>(upper_bound) - std::get<0>(lower_bound),
            std::get<1>(upper_bound) - std::get<1>(lower_bound))
    );
}
