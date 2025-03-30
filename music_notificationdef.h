#ifndef _MUSIC_NOTIFICATIONDEF_H
#define _MUSIC_NOTIFICATIONDEF_H
#include <SDL2/SDL.h>
#include "music_textinfodef.h"

typedef struct music_notificationitem {
    TTF_Font* font;
    char* message;
    text_info_t message_info;
    int padding_x, padding_y;
    SDL_Rect canvas;
    SDL_Color background_color;
    uint64_t timestamp, timeout;
} music_notificationitem_t;

typedef struct music_notificationqueue {
    music_notificationitem_t* items;
    size_t item_count;
} music_notification_t;
#endif