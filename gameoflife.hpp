#ifndef GAME_OF_LIFE_GAME_OF_LIFE_HPP
#define GAME_OF_LIFE_GAME_OF_LIFE_HPP

#include "cell.hpp"

void initialize_rules();
void advance(CellContainer& state, CellContainer& changed_cells);

#endif // GAME_OF_LIFE_GAME_OF_LIFE_HPP
