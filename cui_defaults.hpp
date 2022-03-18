#include <iostream>
#include <unordered_map>

#include "color.hpp"

#include "SDL2/include/SDL2/SDL.h"

#pragma once

SDL_Rect _FILL_RECT_ALL = {0, 0, 10000, 10000};

// Builtin colors.
std::unordered_map<std::string, CUI_Color> colors_to_init = {
    {"white", CUI_Color(255, 255, 254)},
    {"black", CUI_Color(0, 0, 0)},
    {"red", CUI_Color(255, 0, 0)},
    {"orange", CUI_Color(255, 140, 0)},
    {"yellow", CUI_Color(255, 255, 0)},
    {"blue", CUI_Color(0, 0, 255)},
    {"green", CUI_Color(0, 255, 0)},
    {"purple", CUI_Color(230, 230, 250)},
};

CUI_Color CUI_COLOR_WHITE = colors_to_init["white"];
CUI_Color CUI_COLOR_BLACK = colors_to_init["black"];
CUI_Color CUI_COLOR_RED = colors_to_init["red"];
CUI_Color CUI_COLOR_ORANGE = colors_to_init["orange"];
CUI_Color CUI_COLOR_YELLOW = colors_to_init["yellow"];
CUI_Color CUI_COLOR_BLUE = colors_to_init["blue"];
CUI_Color CUI_COLOR_GREEN = colors_to_init["green"];
CUI_Color CUI_COLOR_PURPLE = colors_to_init["purple"];