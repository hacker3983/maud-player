#ifndef _MUSIC_ITEMCONTAINER_H
#define _MUSIC_ITEMCONTAINER_H
#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct music_itemcontainer {
    SDL_Rect* items;
    size_t item_index, item_count;
    bool has_allitems;
} music_itemcontainer_t;

void mplayer_itemcontainer_init(music_itemcontainer_t* item_container);
void mplayer_itemcontainer_set_hasallitems_status(music_itemcontainer_t* item_container, bool status);
void mplayer_itemcontainer_setnextitem_canvas(music_itemcontainer_t* item_container, SDL_Rect canvas);
void mplayer_itemcontainer_additem(music_itemcontainer_t* item_container, SDL_Rect canvas);
void mplayer_itemcontainer_additems(music_itemcontainer_t* item_container, size_t amount);
void mplayer_itemcontainer_popitems(music_itemcontainer_t* item_container, size_t amount);
void mplayer_itemcontainer_resetitem_index(music_itemcontainer_t* item_container);
void mplayer_itemcontainer_destroy(music_itemcontainer_t* item_container);
#endif