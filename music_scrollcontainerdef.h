#ifndef _MUSIC_SCROLLCONTAINERDEF_H
#define _MUSIC_SCROLLCONTAINERDEF_H
#include <SDL2/SDL.h>

typedef struct music_scrollcontainer {
    SDL_Rect* items;
    int scroll_y;
    SDL_Rect scroll_area;
    int scroll_speed;
    size_t content_renderpos, content_count, item_count;
    bool init;
} music_scrollcontainer_t;
#endif