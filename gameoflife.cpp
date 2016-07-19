#include "gameoflife.hpp"
#include "arraysize.hpp"

unsigned int count_bits_set(unsigned int value) {
    unsigned int count;

    for(count = 0; value; ++count) {
      value &= value - 1; // clear the least significant bit set
    }

    return count;
}

CellOffset neighbor_offsets[8] {
    CellOffset(-1, 1),
    CellOffset(0, 1),
    CellOffset(1, 1),
    CellOffset(-1, 0),
    CellOffset(1, 0),
    CellOffset(-1, -1),
    CellOffset(0, -1),
    CellOffset(1, -1),
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

void advance(CellContainer& state, CellContainer& changed_cells) {
    CellContainer to_add;
    CellContainer to_remove;

    for(const Cell& cell: changed_cells) {
        unsigned int mask = neighbor_mask(state, cell);

        if(state.find(cell) != state.end() && cell_die_lookup[mask]) {
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

    const auto add_neighbors_to_changed = [&](const Cell& cell) {
        for(std::size_t i = 0; i < potential_neighbor_count; ++i) {
            Cell neighbor(cell.offset(neighbor_offsets[i]));

            if(state.find(neighbor) != state.end()) {
                changed_cells.insert(neighbor);
            }
        }
    };

    CellContainer::const_iterator current_cell = changed_cells.cbegin();

    while(current_cell != changed_cells.end()) {
        const CellContainer& container = state.find(*current_cell) != state.end()
            ? to_add
            : to_remove;

        if(container.find(*current_cell) == container.end()) {
            current_cell = changed_cells.erase(current_cell); // Erase returns the next cell
        } else {
            ++current_cell;
        }
    }

    for(const Cell& cell: to_add) {
        state.insert(cell);
        changed_cells.insert(cell);
        add_neighbors_to_changed(cell);
    }

    for(const Cell& cell: to_remove) {
        state.erase(cell);
        changed_cells.insert(cell);
        add_neighbors_to_changed(cell);
    }
}
