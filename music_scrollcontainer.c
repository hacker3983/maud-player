#include "music_scrollcontainer.h"

void mplayer_scrollcontainer_init(music_scrollcontainer_t* container) {
    container->init = true;
}

void mplayer_scrollcontainer_setprops(music_scrollcontainer_t* container, SDL_Rect scroll_area,
    int scroll_speed, size_t content_renderpos, size_t content_count) {
    if(container->init) {
        return;
    }
    //container->content_renderpos = content_renderpos;
    container->content_count = content_count;
    container->items = NULL;
    container->item_count = 0;
    container->scroll_y = scroll_area.y;
    container->scroll_area = scroll_area;
    container->scroll_speed = scroll_speed;
}

void mplayer_scrollcontainer_appenditem(music_scrollcontainer_t* container, SDL_Rect scroll_item) {
    if(container->init) {
        return;
    }
    SDL_Rect* new_ptr = NULL;
    // If the container is not initialized then we initialize it and exit the function
    if(!container->items) {
        new_ptr = malloc(sizeof(SDL_Rect));
    } else {
        new_ptr = realloc(container->items, (container->item_count+1) * sizeof(SDL_Rect));
    }
    // When a memory allocation failure occurs then we just exit the function
    if(!new_ptr) {
        return;
    }
    new_ptr[container->item_count++] = scroll_item;
    container->items = new_ptr;
}

bool mplayer_scrollcontainer_hover_scrollarea(mplayer_t* mplayer, music_scrollcontainer_t container) {
    if(mplayer_rect_hover(mplayer, container.scroll_area)) {
        return true;
    }
    return false;
}

void mplayer_scrollcontainer_performscroll(mplayer_t* mplayer, music_scrollcontainer_t* container) {
    if(!mplayer_scrollcontainer_hover_scrollarea(mplayer, *container)) {
        return;
    }
    if(!container->init) {
        return;
    }
    if(!container->items) {
        return;
    }
    if(mplayer->scroll_type == MPLAYERSCROLL_DOWN) {
        int disappear_y = container->scroll_area.y - container->items[0].h;
        if(container->content_renderpos + container->item_count < container->content_count) {
            container->scroll_y -= container->scroll_speed;
            if(container->scroll_y <= disappear_y) {
                container->content_renderpos++;
                free(container->items);
                container->items = NULL;
                container->init = false;
            }
        }
    } else if(mplayer->scroll_type == MPLAYERSCROLL_UP) {
        if(container->scroll_y == container->scroll_area.y && container->content_renderpos) {
            container->scroll_y = container->scroll_area.y - container->items[0].h;
        }
        if(mplayer->scroll_y < container->scroll_area.y) {
            container->scroll_y += container->scroll_speed;
            if(container->scroll_y >= container->scroll_area.y) {
                container->scroll_y = container->scroll_area.y;
                if(container->content_renderpos) {
                    container->content_renderpos--;
                }
            }
        } else if(container->content_renderpos) {
            container->content_renderpos--;
        }
    }
}

void mplayer_scrollcontainer_destroy(music_scrollcontainer_t* container) {
    container->content_renderpos = 0;
    container->content_count = 0;
    free(container->items);
    container->items = NULL;
    container->item_count = 0;
    container->scroll_area.x = 0, container->scroll_area.y = 0,
    container->scroll_area.h = 0, container->scroll_area.w = 0;
    container->init = false;
}