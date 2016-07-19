#ifndef GAME_OF_LIFE_CONSTANTS_HPP
#define GAME_OF_LIFE_CONSTANTS_HPP

#include <SFML/Graphics.hpp>

#ifdef __APPLE__  // TODO: Hack - can't easily detect Retina without linking Objective-C from this app
const unsigned int VIDEO_MODE_WIDTH = 2048, VIDEO_MODE_HEIGHT = 1536;
const unsigned int FLASH_MESSAGE_OFFSET_X = 16;
const unsigned int FLASH_MESSAGE_OFFSET_Y = 12;
const unsigned int FLASH_MESSAGE_CHARACTER_SIZE = 30;
const unsigned int CELL_SHAPE_SIZE = 16;
const unsigned int CELL_GRID_SIZE = CELL_SHAPE_SIZE + 2;
#else
const unsigned int VIDEO_MODE_WIDTH = 1024, VIDEO_MODE_HEIGHT = 768;
const unsigned int FLASH_MESSAGE_OFFSET_X = 8;
const unsigned int FLASH_MESSAGE_OFFSET_Y = 6;
const unsigned int FLASH_MESSAGE_CHARACTER_SIZE = 15;
const unsigned int CELL_SHAPE_SIZE = 8;
const unsigned int CELL_GRID_SIZE = CELL_SHAPE_SIZE + 1;
#endif

const unsigned int VIEWPORT_FIT_CELL_MARGIN = 6;
const sf::Color CELL_COLOR(150, 50, 250);
const sf::Color FLASH_MESSAGE_COLOR(sf::Color::Yellow);
const sf::Int32 MILLISECONDS_AUTO_FIT_FREQUENCY = 1000;
const sf::Int32 MILLISECONDS_PER_GENERATION = 1000 / 60;
const sf::Int32 SLOW_MOTION_MILLISECONDS_PER_GENERATION = 1000 / 2;
const sf::Int32 MILLISECONDS_DISPLAY_FLASH_MESSAGE = 1000 * 3;
const sf::Int32 MILLISECONDS_FADE_FLASH_MESSAGE = 1000 * 2 / 3;

#endif // GAME_OF_LIFE_CONSTANTS_HPP
