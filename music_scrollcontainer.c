#include "music_scrollcontainer.h"

void mplayer_scrollcontainer_setprops(music_scrollcontainer_t* container, SDL_Rect scroll_area,
    int scroll_speed) {
    container->items = NULL;
    container->item_count = 0;
    container->scroll_area = scroll_area;
    container->scroll_speed = scroll_speed;
}

void mplayer_scrollcontainer_appenditem(music_scrollcontainer_t* container, music_scrollcontaineritem_t scroll_item) {
    music_scrollcontaineritem_t* new_ptr = NULL;
    // If the container is not initialized then we initialize it and exit the function
    if(!container->items) {
        new_ptr = malloc(sizeof(music_scrollcontaineritem_t));
    } else {
        new_ptr = realloc(container->items, (container->item_count+1) * sizeof(music_scrollcontaineritem_t));
    }
    // When a memory allocation failure occurs then we just exit the function
    if(!new_ptr) {
        return;
    }
    new_ptr[container->item_count].disappear_y = scroll_item.disappear_y;
    new_ptr[container->item_count].scroll_canvas = scroll_item.initial_canvas;
    new_ptr[container->item_count].initial_canvas = scroll_item.initial_canvas;
    new_ptr[container->item_count].vertical_scroll = scroll_item.vertical_scroll;
    new_ptr[container->item_count++].render = true;
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
    
    bool should_scroll = false;
    if(mplayer->scroll_type == MPLAYERSCROLL_UP) {
        bool startpos_init = false;
        size_t startpos = 0;
        for(size_t i=0;i<container->item_count;i++) {
            if(container->items[i].scroll_canvas.y < container->scroll_area.y) {
                should_scroll = true;
            }
            if(!container->items[i].render) {
                continue;
            }
            if(!startpos_init) {
                startpos = i;
                startpos_init = true;
                break;
            }
        }
        if(!should_scroll) {
            return;
        }
        if(container->items[startpos].scroll_canvas.y < container->items[startpos].initial_canvas.y) {
            container->items[startpos].scroll_canvas.y += container->scroll_speed;
        }
    } else if(mplayer->scroll_type == MPLAYERSCROLL_DOWN) {
        bool startpos_init = false;
        size_t startpos = 0;
        for(size_t i=0;i<container->item_count;i++) {
            if(!container->items[i].render) {
                continue;
            }
            if(!startpos_init) {
                startpos = i;
                startpos_init = true;
            }
            if(container->items[i].scroll_canvas.y + container->items[i].scroll_canvas.h >
                container->scroll_area.y + container->scroll_area.h) {
                should_scroll = true;
                break;
            }
        }
        if(!should_scroll) {
            return;
        }
        printf("startpos: %zu\n", startpos);
        for(size_t i=startpos;i<container->item_count;i++) {
            if(container->items[i].scroll_canvas.y > container->items[i].disappear_y) {
                container->items[i].scroll_canvas.y -= container->scroll_speed;
                if(container->items[i].scroll_canvas.y <= container->items[i].disappear_y) {
                    container->items[i].scroll_canvas.y = container->items[i].disappear_y;
                    container->items[i].render = false;
                    printf("render flag was set to false\n");
                }
            }
        }
    }
}

void mplayer_scrollcontainer_printinfo(music_scrollcontainer_t container) {
    printf("[\n");
    for(size_t i=0;i<container.item_count;i++) {
        music_scrollcontaineritem_t item = container.items[i];
        printf("container.items[%zu] = {\n"
            "\t.initial_canvas = {.w = %d, .h = %d, .x = %d, .y = %d},\n"
            "\t.scroll_canvas = {.w = %d, .h = %d, .x = %d, .y = %d},\n"
            "\t.disappear_y = %d,\n"
            "\t.render = %s\n"
            "}",
            i,
            item.initial_canvas.w, item.initial_canvas.h, item.initial_canvas.x, item.initial_canvas.y,
            item.scroll_canvas.w, item.scroll_canvas.h, item.scroll_canvas.x, item.scroll_canvas.y,
            item.disappear_y,
            (item.render) ? "True" : "False"
        );
        if(i != container.item_count-1) {
            printf(",\n");
        }
    }
    printf("\n]\n");
}
void mplayer_scrollcontainer_destroy(music_scrollcontainer_t* container) {
    free(container->items);
    container->items = NULL;
    container->item_count = 0;
    container->scroll_area.x = 0, container->scroll_area.y = 0,
    container->scroll_area.h = 0, container->scroll_area.w = 0;
    container->init = false;
}