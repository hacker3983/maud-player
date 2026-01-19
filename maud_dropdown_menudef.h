#ifndef _MAUD_DROPDOWN_MENUDEF_H
#define _MAUD_DROPDOWN_MENUDEF_H
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "maud_textinfodef.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct maud_dropdown_item {
    TTF_Font* font;
    SDL_Rect canvas;
    SDL_Color color;
    char* icon_path;
    SDL_Texture* icon_texture;
    SDL_Rect icon_canvas;
    text_info_t name;
    bool clicked;
} maud_dropdown_item_t;

typedef struct maud_dropdown_menu {
    SDL_Rect canvas;
    SDL_Color color;
    int textspace_x, textspace_y;
    int iconspace_x, iconspace_y;
    int icon_width, icon_height;
    maud_dropdown_item_t* items;
    size_t item_count;
} maud_dropdown_menu_t;
#endif