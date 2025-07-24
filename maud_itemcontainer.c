#include "music_itemcontainer.h"

void mplayer_itemcontainer_init(music_itemcontainer_t* item_container) {
    item_container->items = NULL;
    item_container->item_index = 0;
    item_container->item_count = 0;
    item_container->has_allitems = false;
}

void mplayer_itemcontainer_setnextitem_canvas(music_itemcontainer_t* item_container, SDL_Rect canvas) {
    if(item_container->items && item_container->item_index < item_container->item_count) {
        item_container->items[item_container->item_index++] = canvas;
    }
}

void mplayer_itemcontainer_set_hasallitems_status(music_itemcontainer_t* item_container, bool status) {
    item_container->has_allitems = status;
}

void mplayer_itemcontainer_additem(music_itemcontainer_t* item_container, SDL_Rect canvas) {
    if(item_container->has_allitems) {
        return;
    }
    mplayer_itemcontainer_additems(item_container, 1);
    item_container->items[item_container->item_count-1] = canvas;
}

void mplayer_itemcontainer_additems(music_itemcontainer_t* item_container, size_t amount) {
    if(item_container->has_allitems) {
        printf("Adding item from item container function\n");
        return;
    }
    SDL_Rect* new_items = NULL;
    size_t new_itemcount = 0;
    if(!item_container->items) {
        new_items = calloc(amount, sizeof(SDL_Rect));
        new_itemcount = amount;
    } else {
        new_itemcount = item_container->item_count + amount;
        new_items = realloc(item_container->items, new_itemcount * sizeof(SDL_Rect));
        size_t offset = new_itemcount - item_container->item_count;
        memset(new_items + item_container->item_count, 0, offset * sizeof(SDL_Rect));
    }
    if(!new_items) {
        puts("Returning cause new_items is NULL\n");
        return;
    }
    item_container->items = new_items;
    item_container->item_count = new_itemcount;
    printf("Successfully added item\n");
}

void mplayer_itemcontainer_popitems(music_itemcontainer_t* item_container, size_t amount) {
    if((amount > item_container->item_count) ||
        (!item_container->items) || (!amount)) {
        return;
    }
    SDL_Rect* new_items = NULL;
    size_t new_itemcount = item_container->item_count - amount;
    new_items = realloc(item_container, new_itemcount * sizeof(SDL_Rect));
    item_container->items = new_items;
    item_container->item_count = new_itemcount;
}

void mplayer_itemcontainer_resetitem_index(music_itemcontainer_t* item_container) {
    item_container->item_index = 0;
}

void mplayer_itemcontainer_destroy(music_itemcontainer_t* item_container) {
    free(item_container->items);
    mplayer_itemcontainer_init(item_container);
}