#ifndef _MAUD_CHECKBOXES_H
#define _MAUD_CHECKBOXES_H
#include "maud_player.h"

typedef struct maud_checkbox_info {
    SDL_Rect checkbox_canvas;
    SDL_Color fill_color; // checkbox COlor
    SDL_Color box_color; // Checkbox Color
    SDL_Color tk_color;
    bool tick, fill;
} mcheckbox_t;

bool maud_checkbox_hovered(maud_t* maud);
void maud_drawcheckbox(maud_t* maud, mcheckbox_t* checkbox_info);
void maud_drawmusic_checkbox(maud_t* maud, SDL_Color box_color, SDL_Color fill_color,
    bool fill, SDL_Color tick_color, bool check);
#endif