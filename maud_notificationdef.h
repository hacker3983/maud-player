#ifndef _MAUD_NOTIFICATIONDEF_H
#define _MAUD_NOTIFICATIONDEF_H
#include <SDL2/SDL.h>
#include "maud_textinfodef.h"

typedef struct maud_notificationitem {
    TTF_Font* font;
    char* message;
    text_info_t message_info,
                *message_segments;
    size_t message_segmentcount;
    int message_spacing;
    int padding_x, padding_y;
    SDL_Rect canvas;
    SDL_Color background_color;
    double timeout_secs;
    uint64_t timeout;
} maud_notificationitem_t;

typedef struct maud_notificationqueue {
    maud_notificationitem_t* items;
    size_t item_count;
} maud_notification_t;
#endif