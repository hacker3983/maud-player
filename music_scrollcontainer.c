#include "music_scrollcontainer.h"
#include "music_itemcontainer.h"

void mplayer_scrollcontainer_init(music_scrollcontainer_t* container) {
    container->init = true;
}

void mplayer_scrollcontainer_setcontent_count(music_scrollcontainer_t* container, size_t content_count) {
    container->content_count = content_count;
}

void mplayer_scrollcontainer_setscroll_area(music_scrollcontainer_t* container, SDL_Rect scroll_area) {
    container->scroll_area = scroll_area;
}

void mplayer_scrollcontainer_setscroll_y(music_scrollcontainer_t* container, int scroll_y) {
    container->scroll_y = scroll_y;
}

void mplayer_scrollcontainer_setscroll_yfromscroll_area(music_scrollcontainer_t* container) {
    container->scroll_y = container->scroll_area.y;
}

void mplayer_scrollcontainer_setscroll_speed(music_scrollcontainer_t* container, int scroll_speed) {
    container->scroll_speed = scroll_speed;
}

void mplayer_scrollcontainer_setprops(music_scrollcontainer_t* container, SDL_Rect scroll_area,
    int scroll_speed, size_t content_count) {
    if(container->init) {
        return;
    }
    container->content_count = content_count;
    container->scroll_y = scroll_area.y;
    container->scroll_area = scroll_area;
    container->scroll_speed = scroll_speed;
}

void mplayer_scrollcontainer_setnext_itemcanvas(music_scrollcontainer_t* container, SDL_Rect canvas) {
    if(container->init) {
        mplayer_itemcontainer_setnextitem_canvas(&container->item_container, canvas);
    }
}

void mplayer_scrollcontainer_resetitem_index(music_scrollcontainer_t* container) {
    mplayer_itemcontainer_resetitem_index(&container->item_container);
}

void mplayer_scrollcontainer_additem(music_scrollcontainer_t* container, SDL_Rect scroll_item) {
    if(container->init && !container->update) {
        return;
    }
    mplayer_itemcontainer_additems(&container->item_container, 1);
    mplayer_itemcontainer_setnextitem_canvas(&container->item_container, scroll_item);
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
        if(container->item_container.items) {
            printf("Init flag is false but items is not NULL\n");
        }
        if(container->item_container.item_count) {
            printf("Init flag is false but item count is %zu\n", container->item_container.item_count);
        }
        return;
    }
    if(!container->item_container.items) {
        printf("Perform scroll line 55, container->item_container.item_count = %zu\n",
        container->item_container.item_count);
        return;
    }
    if(mplayer->scroll_type == MPLAYERSCROLL_DOWN) {
        int disappear_y = container->scroll_area.y - container->item_container.items[0].h;
        printf("Perform scroll line 58, container->content_renderpos: %zu, container->content_count: %zu, item_count: %zu\n",
            container->content_renderpos, container->content_count, container->item_container.item_count);
        printf("container->content_renderpos + container->item_count = %zu\n", container->content_renderpos + container->item_container.item_count);
        if(container->content_renderpos + container->item_container.item_count < container->content_count) {
            container->scroll_y -= container->scroll_speed;
            printf("Container->scroll_y = %d\n", container->scroll_y);
            if(container->scroll_y <= disappear_y) {
                container->content_renderpos++;
                free(container->item_container.items);
                container->item_container.items = NULL;
                container->item_container.item_count = 0;
                container->item_container.item_index = 0;
                container->init = false;
            }
        } else if(container->item_container.items[container->item_container.item_count-1].y + container->item_container.items[container->item_container.item_count-1].h
            > container->scroll_area.y + container->scroll_area.h) {
            int disappear_y = container->scroll_area.y - container->item_container.items[0].h;
            container->scroll_y -= container->scroll_speed;
            if(container->scroll_y <= disappear_y) {
                container->content_renderpos++;
                free(container->item_container.items);
                container->item_container.items = NULL;
                container->item_container.item_count = 0;
                container->item_container.item_index = 0;
                container->init = false;
            }
        }
    } else if(mplayer->scroll_type == MPLAYERSCROLL_UP) {
        printf("Scroll UP\n");
        if(container->scroll_y == container->scroll_area.y && container->content_renderpos) {
            container->scroll_y = container->scroll_area.y - container->item_container.items[0].h;
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
    mplayer_itemcontainer_destroy(&container->item_container);
    container->scroll_area.x = 0, container->scroll_area.y = 0,
    container->scroll_area.h = 0, container->scroll_area.w = 0;
    container->init = false;
}