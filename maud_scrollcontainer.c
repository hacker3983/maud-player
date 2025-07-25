#include "maud_scrollcontainer.h"
#include "maud_itemcontainer.h"

void maud_scrollcontainer_init(maud_scrollcontainer_t* container) {
    container->init = true;
}

void maud_scrollcontainer_setcontent_count(maud_scrollcontainer_t* container, size_t content_count) {
    container->content_count = content_count;
}

void maud_scrollcontainer_setscroll_area(maud_scrollcontainer_t* container, SDL_Rect scroll_area) {
    container->scroll_area = scroll_area;
}

void maud_scrollcontainer_setscroll_y(maud_scrollcontainer_t* container, int scroll_y) {
    container->scroll_y = scroll_y;
}

void maud_scrollcontainer_setscroll_yfromscroll_area(maud_scrollcontainer_t* container) {
    container->scroll_y = container->scroll_area.y;
}

void maud_scrollcontainer_setscroll_speed(maud_scrollcontainer_t* container, int scroll_speed) {
    container->scroll_speed = scroll_speed;
}

void maud_scrollcontainer_setprops(maud_scrollcontainer_t* container, SDL_Rect scroll_area,
    int scroll_speed, size_t content_count) {
    if(container->init) {
        return;
    }
    container->content_count = content_count;
    container->scroll_y = scroll_area.y;
    container->scroll_area = scroll_area;
    container->scroll_speed = scroll_speed;
}

void maud_scrollcontainer_setnext_itemcanvas(maud_scrollcontainer_t* container, SDL_Rect canvas) {
    if(container->init) {
        maud_itemcontainer_setnextitem_canvas(&container->item_container, canvas);
    }
}

void maud_scrollcontainer_resetitem_index(maud_scrollcontainer_t* container) {
    maud_itemcontainer_resetitem_index(&container->item_container);
}

void maud_scrollcontainer_additem(maud_scrollcontainer_t* container, SDL_Rect scroll_item) {
    if(container->init && !container->update) {
        return;
    }
    maud_itemcontainer_additems(&container->item_container, 1);
    maud_itemcontainer_setnextitem_canvas(&container->item_container, scroll_item);
}

bool maud_scrollcontainer_hover_scrollarea(maud_t* maud, maud_scrollcontainer_t container) {
    if(maud_rect_hover(maud, container.scroll_area)) {
        return true;
    }
    return false;
}

void maud_scrollcontainer_performscroll_overscrollarea(maud_t* maud, maud_scrollcontainer_t* container) {
    if(!maud_scrollcontainer_hover_scrollarea(maud, *container)) {
        return;
    }    
    maud_scrollcontainer_performscroll(maud, container, container->scroll_area.y);
}

void maud_scrollcontainer_performscroll(maud_t* maud, maud_scrollcontainer_t* container, int scroll_y) {
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
    if(maud->scroll_type == MAUDSCROLL_DOWN) {
        int disappear_y = scroll_y - container->item_container.items[0].h;
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
            > scroll_y + container->scroll_area.h) {
            int disappear_y = scroll_y - container->item_container.items[0].h;
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
    } else if(maud->scroll_type == MAUDSCROLL_UP) {
        printf("Scroll UP\n");
        if(container->scroll_y == scroll_y && container->content_renderpos) {
            container->scroll_y = container->scroll_y - container->item_container.items[0].h;
        }
        if(maud->scroll_y < scroll_y) {
            container->scroll_y += container->scroll_speed;
            if(container->scroll_y >= scroll_y) {
                container->scroll_y = scroll_y;
                if(container->content_renderpos) {
                    container->content_renderpos--;
                }
            }
        } else if(container->content_renderpos) {
            container->content_renderpos--;
        }
    }
}

void maud_scrollcontainer_destroy(maud_scrollcontainer_t* container) {
    container->content_renderpos = 0;
    container->content_count = 0;
    maud_itemcontainer_destroy(&container->item_container);
    container->scroll_area.x = 0, container->scroll_area.y = 0,
    container->scroll_area.h = 0, container->scroll_area.w = 0;
    container->init = false;
}