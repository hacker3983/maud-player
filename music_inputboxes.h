#ifndef _MUSIC_INPUTBOXES_H
#define _MUSIC_INPUTBOXES_H
#include "music_player.h"
#include "music_inputboxesdef.h"
#include "music_textmanager.h"
#include "music_string.h"

bool mplayer_inputdata_realloc_datalist(mplayer_inputdata_t** input_datalist, size_t* input_datacount);
mplayer_inputbox_t mplayer_inputbox_create(TTF_Font* input_datafont, TTF_Font* placeholder_font,
    SDL_Rect inputbox_canvas, const char* placeholder, SDL_Color placeholder_color, SDL_Color inputbox_color,
    SDL_Color input_datacolor, SDL_Color cursor_color);
bool mplayer_inputbox_realloc_datalist(mplayer_inputbox_t* input_box);
bool mplayer_inputbox_realloc_datalistby(mplayer_inputbox_t* input_box, size_t amount);
void mplayer_inputbox_renderplaceholder(mplayer_t* mplayer, mplayer_inputbox_t* input_box);
void mplayer_inputbox_renderinputdata(mplayer_t* mplayer, mplayer_inputbox_t* input_box);
void mplayer_inputbox_rendercursor(mplayer_t* mplayer, mplayer_inputbox_t* input_box);
void mplayer_inputbox_display(mplayer_t* mplayer, mplayer_inputbox_t* input_box);
bool mplayer_inputbox_initialize_input_datalist(mplayer_inputbox_t* input_box);
bool mplayer_inputbox_addinput_data(mplayer_inputbox_t* input_box, const char* input_data);
void mplayer_inputbox_handleinputs(mplayer_t* mplayer, mplayer_inputbox_t* input_box);
void mplayer_inputbox_shiftdatalist_fromcursorpos(mplayer_inputbox_t* input_box);
void mplayer_inputbox_clear(mplayer_inputbox_t* input_box);
void mplayer_inputdata_destroy(mplayer_inputdata_t** input_datalist, size_t* input_datacount);
void mplayer_inputbox_destroy(mplayer_inputbox_t* input_box);
#endif