#ifndef GAME_OF_LIFE_RESOURCES_HPP
#define GAME_OF_LIFE_RESOURCES_HPP

#include <string>
#include <SFML/Graphics.hpp>
#include "cell.hpp"

void load_state(const std::string& filename, CellContainer& state);
sf::Font load_font(const std::string& filename);

#endif // GAME_OF_LIFE_RESOURCES_HPP
