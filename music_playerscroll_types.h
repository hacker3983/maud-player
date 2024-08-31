#ifndef _MUSIC_PLAYERSCROLL_TYPES_H
#define _MUSIC_PLAYERSCROLL_TYPES_H
#include <SDL2/SDL.h>

enum scroll_types {
    MPLAYERSCROLL_UP,
    MPLAYERSCROLL_DOWN,
    MPLAYERSCROLL_RIGHT,
    MPLAYERSCROLL_LEFT
};

int mplayer_scrolltype_getmousewheel_scrolltype(SDL_Event e, int* scroll_typeptr);
#endif