#ifndef _MUSIC_ITEMCONTAINER_H
#define _MUSIC_ITEMCONTAINER_H
#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct maud_itemcontainer {
    SDL_Rect* items;
    size_t item_index, item_count;
    bool has_allitems;
} maud_itemcontainer_t;

void maud_itemcontainer_init(maud_itemcontainer_t* item_container);
void maud_itemcontainer_set_hasallitems_status(maud_itemcontainer_t* item_container, bool status);
void maud_itemcontainer_setnextitem_canvas(maud_itemcontainer_t* item_container, SDL_Rect canvas);
void maud_itemcontainer_additem(maud_itemcontainer_t* item_container, SDL_Rect canvas);
void maud_itemcontainer_additems(maud_itemcontainer_t* item_container, size_t amount);
void maud_itemcontainer_popitems(maud_itemcontainer_t* item_container, size_t amount);
void maud_itemcontainer_resetitem_index(maud_itemcontainer_t* item_container);
void maud_itemcontainer_destroy(maud_itemcontainer_t* item_container);
#endif