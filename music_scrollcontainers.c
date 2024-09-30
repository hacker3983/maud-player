#include "music_scrollcontainers.h"
#include "music_playerscroll_types.h"

bool mplayer_scrollcontainer_hover_scrollarea(mplayer_scrollcontainer_areainfo_t scrollarea_info) {
    int mouse_x = scrollarea_info.mouse_x, mouse_y = scrollarea_info.mouse_y;
    SDL_Rect scroll_area = scrollarea_info.scroll_area;
    if((mouse_x <= scroll_area.x + scroll_area.w && mouse_x >= scroll_area.x) &&
        (mouse_y <= scroll_area.y + scroll_area.h && mouse_y >= scroll_area.y)) {
        return true;
    }
    return false;
}

mplayer_scrollcontainer_t mplayer_scrollcontainer_create(SDL_Rect initial_canvas,
    int disappear_y, bool vertical_scroll, bool render) {
    mplayer_scrollcontainer_t scroll_container = {
        .initial_canvas = initial_canvas,
        .scroll_canvas = initial_canvas,
        .disappear_y = disappear_y,
        .vertical_scroll = vertical_scroll,
        .render = render
    };
    return scroll_container;
}

void mplayer_scrollcontainer_append(mplayer_scrollcontainer_t** containers, mplayer_scrollcontainer_t container,
    size_t* container_count) {
    mplayer_scrollcontainer_t* temp = NULL;
    size_t container_acc = *container_count;
    if(!(*containers) || !container_acc) {
        temp = (mplayer_scrollcontainer_t*)malloc(sizeof(mplayer_scrollcontainer_t));
    } else {
        temp = (mplayer_scrollcontainer_t*)realloc(*containers,
                (container_acc+1) * sizeof(mplayer_scrollcontainer_t));
    }
    if(!temp) {
        fprintf(stderr, "mplayer_scrollcontainer_append(): Failed because a memory allocation failure happened\n");
        return;
    }
    *containers = temp;
    (*containers)[container_acc] = container;
    container_acc++;
    *container_count = container_acc;
}

void mplayer_scrollcontainer_printinfo(mplayer_scrollcontainer_t container) {
    SDL_Rect scroll_canvas = container.scroll_canvas, initial_canvas = container.initial_canvas;
    printf("container.initial_canvas = {.w = %d, .h = %d, .x = %d. y = %d}\n"
           "container.scroll_canvas = {.w = %d, .h = %d, .x = %d, .y = %d}\n"
           "container.disappear_y = %d\n"
           "container.vertical_scroll = %s\n"
           "container.render = %s\n",
           initial_canvas.w, initial_canvas.h, initial_canvas.x, initial_canvas.y,
           scroll_canvas.w, scroll_canvas.h, scroll_canvas.x, scroll_canvas.y,
           container.disappear_y,
           container.vertical_scroll ? "True" : "False",
           container.render ? "True" : "False");
}

void mplayer_scrollcontainer_performscroll(mplayer_scrollcontainer_t* container, int scroll_type, int scroll_speed,
    mplayer_scrollcontainer_areainfo_t scrollarea_info) {
    // If the mouse is not in the range of a particular scroll container then we do not perform any scrolling operations
    if(!mplayer_scrollcontainer_hover_scrollarea(scrollarea_info)) {
        return;
    }
    switch(scroll_type) {
        case MPLAYERSCROLL_DOWN:
            printf("Scroll Down\n");
            if(container->scroll_canvas.y + container->scroll_canvas.h >= container->disappear_y) {
                container->scroll_canvas.y -= scroll_speed;
            } else {
                container->render = false;
            }
            break;
        case MPLAYERSCROLL_UP:
            printf("Scroll Up\n");
            if(container->scroll_canvas.y + container->scroll_canvas.h <= container->initial_canvas.y +
                container->initial_canvas.h) {
                container->scroll_canvas.y += scroll_speed;
                if(container->scroll_canvas.y + container->scroll_canvas.h >= container->initial_canvas.y + container->initial_canvas.h) {
                    container->scroll_canvas.y = container->initial_canvas.y;
                } else {
                    container->render = true;
                }
            }
            break;
    }
}

void mplayer_scrollcontainers_performscroll(mplayer_scrollcontainer_t* containers, int scroll_type, int scroll_speed,
    size_t container_count, mplayer_scrollcontainer_areainfo_t scrollarea_info) {
    if(!mplayer_scrollcontainer_hover_scrollarea(scrollarea_info)) {
        return;
    }
    int total_containerheight = 0;
    size_t start_scrollpos = 0, end_scrollpos = 0;
    bool initialized_scrollpos = false;
    for(size_t i=0;i<container_count;i++) {
        if(!containers[i].render) {
            continue;
        }
        if(!initialized_scrollpos) {
            start_scrollpos = i;
            initialized_scrollpos = true;
        }
        end_scrollpos = i+1;
        if(containers[i].scroll_canvas.y + containers[i].scroll_canvas.h
            >= scrollarea_info.scroll_area.y + scrollarea_info.scroll_area.h) {
            break;
        }
    }
    for(size_t i=start_scrollpos;i<end_scrollpos && end_scrollpos-1 != container_count;i++) {
        mplayer_scrollcontainer_performscroll(&containers[i], scroll_type, scroll_speed, scrollarea_info);
    }
    printf("start_scrollpos: %d, end_scrollpos: %d\n", start_scrollpos, end_scrollpos);
}

void mplayer_scrollcontainer_destroylist(mplayer_scrollcontainer_t** containers, size_t* container_count) {
    *container_count = 0;
    free(*containers); *containers = NULL;
}