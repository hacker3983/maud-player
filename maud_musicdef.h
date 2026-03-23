#ifndef _MAUD_MUSICDEF_H
#define _MAUD_MUSICDEF_H
#include <stddef.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "maud_textinfodef.h"

// Music time
typedef struct music_time {
    int hrs, mins, secs;
} mtime_t;

// structure representing music
typedef struct music {
    char *music_name, *location_path,
         *file_path;
    size_t music_id;
    text_info_t text_info;
    SDL_Texture* text_texture;
    size_t searchmusic_id;
    Mix_Music* music;
    mtime_t music_position, music_duration;
    double music_positionsecs, music_durationsecs;
    SDL_Rect outer_canvas, checkbox_size;
    bool checkbox_ticked, fill, fit,
        remove;
} music_t;
#endif