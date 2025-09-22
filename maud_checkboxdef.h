#ifndef _MAUD_CHECKBOXDEF_H
#define _MAUD_CHECKBOXDEF_H
#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct maud_checkbox_tickline {
    int x1, y1,
        x2, y2;
} maud_checkbox_tickline_t;

typedef struct maud_checkbox_tick {
    maud_checkbox_tickline_t shortline;
    maud_checkbox_tickline_t longline;
} maud_checkbox_tick_t;

typedef struct maud_checkbox {
    SDL_Rect canvas;
    maud_checkbox_tick_t tick_line;
    SDL_Color color, fill_color,
              tick_color;
    bool tick, fill;
} maud_checkbox_t;
#endif