#ifndef _MUSIC_SCROLLCONTAINERDEF_H
#define _MUSIC_SCROLLCONTAINERDEF_H
#include <SDL2/SDL.h>
typedef struct music_scrollcontainer_item {
    SDL_Rect scroll_canvas; // The canvas that we should scroll on
    SDL_Rect initial_canvas; // The intial position for the canvas this will be used to restore the canvas position
    int disappear_y; // The y position on the screen to disappear or stop rendering the
                     // content on screen
    bool vertical_scroll; // True for vertical scroll and False for horizontal scroll
    bool render;
} music_scrollcontaineritem_t;

typedef struct music_scrollcontainer {
    music_scrollcontaineritem_t* items;
    SDL_Rect scroll_area;
    int scroll_speed;
    size_t item_count;
    bool init;
} music_scrollcontainer_t;
#endif