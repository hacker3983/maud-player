#ifndef _MUSIC_SCROLLCONTAINERDEF_H
#define _MUSIC_SCROLLCONTAINERDEF_H
#include <SDL2/SDL.h>
#include "maud_itemcontainer.h"

typedef struct maud_scrollcontainer {
    maud_itemcontainer_t item_container;
    int scroll_y;
    SDL_Rect scroll_area;
    int scroll_speed;
    size_t content_renderpos, content_count;
    bool init, update;
} maud_scrollcontainer_t;
#endif