#ifndef _MUSIC_INPUTBOXES_H
#define _MUSIC_INPUTBOXES_H
#include "music_player.h"

typedef struct music_inputbox {
    char** input_data;
    char* placeholder;
    size_t input_data;
    SDL_Rect placeholder_canvas;
} mplayer_inputbox_t;

mplayer_inputbox_t mplayer_inputbox_create(const char* placeholder);
void mplayer_inputbox_destroy(mplayer_inputbox_t* input_box);
#endif