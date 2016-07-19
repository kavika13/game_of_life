#ifndef GAME_OF_LIFE_CONSTANTS_HPP
#define GAME_OF_LIFE_CONSTANTS_HPP

#include <SFML/Graphics.hpp>

#ifdef __APPLE__  // TODO: Hack - can't easily detect Retina without linking Objective-C from this app
const float DEVICE_PIXEL_RATIO = 2.0;
#else
const float DEVICE_PIXEL_RATIO = 1.0;
#endif

// TODO: These will probably have to be runtime-derived values instead of constants
const unsigned int VIDEO_MODE_WIDTH = 1024 * DEVICE_PIXEL_RATIO, VIDEO_MODE_HEIGHT = 768 * DEVICE_PIXEL_RATIO;
const unsigned int FLASH_MESSAGE_OFFSET_X = 8 * DEVICE_PIXEL_RATIO;
const unsigned int FLASH_MESSAGE_OFFSET_Y = 6 * DEVICE_PIXEL_RATIO;
const unsigned int FLASH_MESSAGE_CHARACTER_SIZE = 30 * DEVICE_PIXEL_RATIO;
const unsigned int CELL_SHAPE_SIZE = 8 * DEVICE_PIXEL_RATIO;
const unsigned int CELL_GRID_SIZE = CELL_SHAPE_SIZE + 1 * DEVICE_PIXEL_RATIO;

const unsigned int VIRTUAL_RESOLUTION_WIDTH = VIDEO_MODE_WIDTH / 3 / DEVICE_PIXEL_RATIO;
const unsigned int VIRTUAL_RESOLUTION_HEIGHT = VIDEO_MODE_HEIGHT / 3 / DEVICE_PIXEL_RATIO;

const bool ENABLE_SCANLINES = true;
const bool ENABLE_PIXEL_RASTERIZATION = false;

const bool ENABLE_SCREEN_CURVATURE = true;
const float SCREEN_CURVATURE = 0.25f;

const unsigned int VIEWPORT_FIT_CELL_MARGIN = 6;
const sf::Color CELL_COLOR(150, 50, 250);
const sf::Color FLASH_MESSAGE_COLOR(sf::Color::Yellow);
const sf::Int32 MILLISECONDS_AUTO_FIT_FREQUENCY = 1000;
const sf::Int32 MILLISECONDS_PER_GENERATION = 1000 / 60;
const sf::Int32 SLOW_MOTION_MILLISECONDS_PER_GENERATION = 1000 / 2;
const sf::Int32 MILLISECONDS_DISPLAY_FLASH_MESSAGE = 1000 * 3;
const sf::Int32 MILLISECONDS_FADE_FLASH_MESSAGE = 1000 * 2 / 3;

#endif // GAME_OF_LIFE_CONSTANTS_HPP
