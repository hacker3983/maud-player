#ifndef _MUSIC_TEXTINFODEF_H
#define _MUSIC_TEXTINFODEF_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

typedef struct text_info {
    int font_size;
    char *text, *utext;
    SDL_Color text_color;
    SDL_Rect text_canvas;
} text_info_t;
#endif