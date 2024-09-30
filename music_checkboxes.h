#ifndef _MUSIC_CHECKBOXES_H
#define _MUSIC_CHECKBOXES_H
#include "music_player.h"

typedef struct music_checkbox_info {
    SDL_Rect checkbox_canvas;
    SDL_Color fill_color; // checkbox COlor
    SDL_Color box_color; // Checkbox Color
    SDL_Color tk_color;
    bool tick, fill;
} mcheckbox_t;

bool mplayer_checkbox_hovered(mplayer_t* mplayer);
void mplayer_drawcheckbox(mplayer_t* mplayer, mcheckbox_t* checkbox_info);
void mplayer_drawmusic_checkbox(mplayer_t* mplayer, SDL_Color box_color, SDL_Color fill_color,
    bool fill, SDL_Color tick_color, bool check);
#endif