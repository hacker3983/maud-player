#ifndef _MAUD_CHECKBOXES_H
#define _MAUD_CHECKBOXES_H
#include "maud_player.h"

void maud_checkbox_drawtick_shortline(maud_t* maud, maud_checkbox_t* checkbox);
void maud_checkbox_drawtick_longline(maud_t* maud, maud_checkbox_t* checkbox);
void maud_checkbox_drawtick(maud_t* maud, maud_checkbox_t* checkbox);
void maud_checkbox_draw(maud_t* maud, maud_checkbox_t* checkbox);
void maud_checkbox_drawmusic_checkbox(maud_t* maud, SDL_Color box_color, SDL_Color fill_color,
    bool fill, SDL_Color tick_color, bool check);
#endif