#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>

#include "extern_functions.hpp"
#include "cui_objects.hpp"
#include "font_renderer.hpp"

#include "SDL2/include/SDL2/SDL.h"
#include "SDL2/include/SDL2/SDL_image.h"
#include "SDL2/include/SDL2/SDL_ttf.h"

#pragma once

// SDL Window and Renderer.
SDL_Window* cui_window;
SDL_Renderer* cui_renderer;

// SDL Cursors.
std::unordered_map<std::string, SDL_Cursor*> cui_cursors;

// FPS Capping.
const int _FPS = 60;
const float _FRAME_DELAY = 1000 / _FPS;
Uint32 _framestart;
int _frametime;

// Update variables.
bool mouse_clicked, sent_mouse_clicked;
bool mouse_held, sent_mouse_held;
bool mouse_up, sent_mouse_up;
int scrolled;
bool sent_scrolled;

// Mouse position.
SDL_Rect mouse_rect;

// CUI Objects
// std::vector<CUI_Object*> ui_objects;
std::vector<std::unique_ptr<CUI_Object>> ui_objects;

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
std::string CUI_COLOR_WHITE = "white";
std::string CUI_COLOR_BLACK = "black";
std::string CUI_COLOR_RED = "red";
std::string CUI_COLOR_ORANGE = "orange";
std::string CUI_COLOR_YELLOW = "yellow";
std::string CUI_COLOR_BLUE = "blue";
std::string CUI_COLOR_GREEN = "green";
std::string CUI_COLOR_PURPLE = "purple";

// Initialize SDL2.
void cuiInit(){

    // init sdl
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF);
    TTF_Init();
    int window_width, window_height;
    getDesktopSize(window_width, window_height);
    cui_window = SDL_CreateWindow(
        "CUI Application",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        window_width, window_height,
        SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_BORDERLESS);
    cui_renderer = SDL_CreateRenderer(cui_window, -1, SDL_RENDERER_SOFTWARE);
    SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");

    // make window transparent
    makeWindowTransparent(cui_window, RGB(255, 255, 255));

    // initialize fonts
    for (auto [color_name, color_class]: colors_to_init){
        loadFont(cui_renderer, color_name, "fonts/verdana.ttf", color_class.r, color_class.g, color_class.b);
    }
    
    // create cursors
    cui_cursors["clickable"] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    cui_cursors["text"] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);

}

// Quit SDL2.
void cuiQuit(){
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

// Call this in your main loop.
bool cuiUpdate(){

    // clear update variables
    mouse_clicked = false;
    mouse_held = false;
    mouse_up = false;
    sent_mouse_clicked = false;
    sent_mouse_held = false;
    sent_mouse_up = false;
    scrolled = 0;
    sent_scrolled = false;

    // clear renderer
    SDL_SetRenderDrawColor(cui_renderer, 255, 255, 255, 255);
    SDL_RenderClear(cui_renderer);

    // get mouse rect
    POINT mouse_pos;
    GetCursorPos(&mouse_pos);
    mouse_rect = {mouse_pos.x, mouse_pos.y, 1, 1};

    // check if mouse is held
    if (GetKeyState(VK_LBUTTON) < 0){
        mouse_held = true;
    }

    // handle events
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0){;
        if (event.type == SDL_QUIT){
            return false;
        } else if (event.type == SDL_KEYDOWN){
            // SDL_Keycode key = event.key.keysym.sym;
        } else if (event.type == SDL_MOUSEBUTTONDOWN){ // mouse clicked?
            mouse_clicked = true;
        } else if (event.type == SDL_MOUSEBUTTONUP){ // mouse up?
            mouse_up = true;
        } else if (event.type == SDL_MOUSEWHEEL){
            if (event.wheel.y > 0){
                scrolled = -1;
            } else if (event.wheel.y < 0){
                scrolled = 1;
            }
        }
    }
    
    // send events to objects and update and render
    for (std::unique_ptr<CUI_Object>& object: ui_objects){

        // send events if collided with mouse
        if (object->collides(mouse_rect)){

            // send hold
            if (mouse_held && !sent_mouse_held){
                object->mouseHeld(mouse_rect);
                sent_mouse_held = true;
            }

            // send clicked
            if (mouse_clicked && !sent_mouse_clicked){
                object->clicked(mouse_rect);
                sent_mouse_clicked = true;
            }

            // send mouse up
            if (mouse_up && !sent_mouse_up){
                object->mouseUp(mouse_rect);
                sent_mouse_up = true;
            }

            // send scroll
            if (scrolled != 0 && !sent_scrolled){
                object->scrolled(mouse_rect, scrolled);
                sent_scrolled = true;
            }

        }

        object->update();
    }

    for (int index = ui_objects.size() - 1; index != -1; index --){
        ui_objects[index]->render(cui_renderer);
    }

    // present renderer
    SDL_RenderPresent(cui_renderer);

    // cap fps
    _frametime = SDL_GetTicks() - _framestart;
    if (_FRAME_DELAY > _frametime){
        SDL_Delay(_FRAME_DELAY - _frametime);
    }
    _framestart = SDL_GetTicks();

    return true;
}

// Function to create a window.
CUI_Window* createWindow(std::string name, int x, int y, int width, int height, Uint16 color_r, Uint16 color_g, Uint16 color_b, Uint16 color_a){
    // CUI_Window created_window(name, x, y, width, height, color_r, color_g, color_b, color_a);
    // CUI_Window* window_ptr = &created_window;
    auto window_ptr = std::make_unique<CUI_Window>(name, x, y, width, height, color_r, color_g, color_b, color_a);
    auto returned_ptr = window_ptr.get();
    ui_objects.push_back(std::move(window_ptr));
    return returned_ptr;
}

// Function to add text to a window.
CUI_Text* addText(CUI_Window* window, std::string text_content, float size, std::string color, int nextline){
    auto text_object_ptr = std::make_unique<CUI_Text>(text_content, size, color, nextline);
    auto returned_ptr = text_object_ptr.get();
    window->child_objects.push_back(std::move(text_object_ptr));
    return returned_ptr;
}