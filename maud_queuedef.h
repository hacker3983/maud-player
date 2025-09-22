#ifndef _MAUD_QUEUEDEF_H
#define _MAUD_QUEUEDEF_H
#include <stddef.h>
#include "maud_textinfodef.h"

typedef struct maud_queueitem {
    SDL_Rect canvas;
    text_info_t music_name;
    SDL_Color color;
    SDL_Color checkbox_color;
    SDL_Color checkbox_fillcolor;
    SDL_Color checkbox_tickcolor;
    size_t uid, music_listindex, music_id;
    bool fill, checkbox_ticked;
} maud_queueitem_t;

typedef struct maud_queueprops {
    int scrollarea_startx, scrollarea_starty;
    int scrollstart_y, scroll_y;
    SDL_Rect clip_area, scroll_area;
    size_t *start_renderpos,
            end_renderpos;
    bool check_scrollarea_hover, init;
} maud_queueprops_t;

typedef struct maud_queue {
    maud_queueitem_t* items;
    maud_queueprops_t queue_props;
    size_t start_renderpos,
           playid, item_count;
} maud_queue_t;
#endif