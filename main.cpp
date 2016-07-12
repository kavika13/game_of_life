#include <unordered_set>
#include <cinttypes>
#include <iostream>
#include <map>

// Helper to get the array size statically
template<typename TArrayType, std::size_t TArraySize>
constexpr std::size_t array_size(const TArrayType (&array)[TArraySize]) { return TArraySize; }

typedef std::int64_t CellDimension;
typedef std::tuple<CellDimension, CellDimension> CellOffset;

class Cell {
public:
    Cell(CellDimension x = 0, CellDimension y = 0) : x_(x), y_(y) {}

    Cell offset(const CellOffset& offset) const {
        return Cell(x_ + std::get<0>(offset), y_ + std::get<1>(offset));
    }

    std::size_t get_hash_code() const {
        std::size_t result = 17;
        result = result * 23 + std::hash<CellDimension>()(x_);
        result = result * 23 + std::hash<CellDimension>()(y_);
        return result;
    }

private:
    friend bool operator==(const Cell& lhs, const Cell& rhs);
    friend std::ostream& operator<<(std::ostream&, const Cell&);

    CellDimension x_;
    CellDimension y_;
};

bool operator==(const Cell& lhs, const Cell& rhs) {
    return lhs.x_ == rhs.x_ && lhs.y_ == rhs.y_;
}

std::ostream& operator<<(std::ostream& os, const Cell& cell) {
   return os << "<Cell: " << cell.x_ << ", " << cell.y_ << ">";
}

typedef std::unordered_set<Cell> CellContainer;

namespace std {
    template <> struct hash<Cell> {
        std::size_t operator()(const Cell& cell) const {
            return cell.get_hash_code();
        }
    };
}

std::ostream& operator<<(std::ostream& os, const CellContainer& cells) {
    for(const Cell& cell: cells) {
        os << cell << ", ";
    }
    return os;
}

unsigned int count_bits_set(unsigned int value) {
    unsigned int count;

    for(count = 0; value; ++count) {
      value &= value - 1; // clear the least significant bit set
    }

    return count;
}

CellOffset neighbor_offsets[8] {
    {-1, 1},
    {0, 1},
    {1, 1},
    {-1, 0},
    {1, 0},
    {-1, -1},
    {0, -1},
    {1, -1},
};
const std::size_t potential_neighbor_count = array_size(neighbor_offsets);

const std::size_t lookup_size = 1 << potential_neighbor_count;
bool cell_spawn_lookup[lookup_size];
bool cell_die_lookup[lookup_size];

void initialize_rules() {
    for(unsigned int i = 0; i < lookup_size; ++i) {
        unsigned int count = count_bits_set(i);
        cell_spawn_lookup[i] = count == 3;
        cell_die_lookup[i] = count < 2 || count > 3;
    }
}

unsigned int neighbor_mask(const CellContainer& state, const Cell& cell) {
    unsigned int mask = 0;
    for(std::size_t i = 0; i < potential_neighbor_count; ++i) {
        mask |= state.find(cell.offset(neighbor_offsets[i])) == state.end() ? 0 : 1 << i;
    }
    return mask;
}

void advance(CellContainer& state) {
    CellContainer to_add;
    CellContainer to_remove;

    for(const Cell& cell: state) {
        unsigned int mask = neighbor_mask(state, cell);

        if(cell_die_lookup[mask]) {
            to_remove.insert(cell);
        }

        for(const CellOffset& offset: neighbor_offsets) {
            Cell neighbor = cell.offset(offset);

            if(state.find(neighbor) == state.end()) {
                unsigned int mask = neighbor_mask(state, neighbor);

                if(cell_spawn_lookup[mask]) {
                    to_add.insert(neighbor);
                }
            }
        }
    }

    for(const Cell& cell: to_add) {
        state.insert(cell);
    }
    for(const Cell& cell: to_remove) {
        state.erase(cell);
    }
}

// TODO: Does this template magic really make my life easier here? :)
template<std::size_t cells_count>
void add_shape(CellContainer& state, const Cell (&cells)[cells_count], CellOffset offset = {0, 0}) {
    for(std::size_t i = 0; i < array_size(cells); ++i) {
        state.insert(cells[i].offset(offset));
    }
}

void add_spinner(CellContainer& state, CellOffset offset = {0, 0}) {
    Cell cells[] {
        Cell(0, 0),
        Cell(1, 0),
        Cell(2, 0),
    };
    add_shape(state, cells, offset);
}

void add_block(CellContainer& state, CellOffset offset = {0, 0}) {
    Cell cells[] {
        Cell(0, 0),
        Cell(1, 0),
        Cell(0, 1),
        Cell(1, 1),
    };
    add_shape(state, cells, offset);
}

int main(int argc, char* argv[]) {
    initialize_rules();
    CellContainer state;
    add_spinner(state);
    std::cout << state << "\n";
    advance(state);
    std::cout << state << "\n";
    return 0;
}
