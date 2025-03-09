#ifndef GLOBAL_CONSTANTS_H
#define GLOBAL_CONSTANTS_H

#include <SFML/Graphics.hpp>

using namespace sf;

#define FRAMERATE_LIMIT 60
#define DEBUG false

constexpr Vector2u SCREEN_RESOLUTION = {640, 360};
constexpr Vector2u TILE_SIZE = {16, 16};

const Font GAME_FONT("assets/fonts/VCR_OSD_MONO_1.001.ttf");

#define MENU_INPUT_DELAY 0.4f

#endif