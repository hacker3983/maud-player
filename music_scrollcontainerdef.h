#ifndef _MUSIC_SCROLLCONTAINERDEF_H
#define _MUSIC_SCROLLCONTAINERDEF_H
#include <SDL2/SDL.h>
#include "music_itemcontainer.h"

typedef struct music_scrollcontainer {
    music_itemcontainer_t item_container;
    int scroll_y;
    SDL_Rect scroll_area;
    int scroll_speed;
    size_t content_renderpos, content_count;
    bool init, update;
} music_scrollcontainer_t;
#endif