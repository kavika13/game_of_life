#ifndef GAME_OF_LIFE_RESOURCES_HPP
#define GAME_OF_LIFE_RESOURCES_HPP

#include <string>
#include <SFML/Graphics.hpp>
#include "cell.hpp"

void load_state(const std::string& filename, CellContainer& state);

sf::Font load_font(const std::string& filename);

bool load_vertex_shader(sf::Shader& shader, const std::string& filename);
bool load_fragment_shader(sf::Shader& shader, const std::string& filename);
bool load_full_shader(
    sf::Shader& shader,
    const std::string& vertex_shader_filename,
    const std::string& fragment_shader_filename);

#endif // GAME_OF_LIFE_RESOURCES_HPP
