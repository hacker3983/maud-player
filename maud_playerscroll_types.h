#ifndef _MAUD_PLAYERSCROLL_TYPES_H
#define _MAUD_PLAYERSCROLL_TYPES_H
#include <SDL2/SDL.h>

enum scroll_types {
    MAUDSCROLL_UP,
    MAUDSCROLL_DOWN,
    MAUDSCROLL_RIGHT,
    MAUDSCROLL_LEFT
};

int maud_scrolltype_getmousewheel_scrolltype(SDL_Event e, int* scroll_typeptr);
#endif