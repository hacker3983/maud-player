#ifndef _MUSIC_INPUTBOXES_H
#define _MUSIC_INPUTBOXES_H
#include "music_player.h"
#include "music_inputboxesdef.h"
#include "music_textmanager.h"
#include "music_string.h"

bool mplayer_inputdata_addbyte_position(input_data_t* input, size_t start_index, size_t end_index);
bool mplayer_inputbox_getinput_beforecursor(mplayer_inputbox_t* input_box, input_data_t* input_before);
bool mplayer_inputbox_getinput_aftercursor(mplayer_inputbox_t* input_box, input_data_t* input_after);
mplayer_inputbox_t mplayer_inputbox_create(TTF_Font* input_datafont, TTF_Font* placeholder_font,
    SDL_Rect inputbox_canvas, const char* placeholder, SDL_Color placeholder_color, SDL_Color inputbox_color,
    SDL_Color input_datacolor, SDL_Color cursor_color);
void mplayer_inputbox_getplaceholder_size(mplayer_t* mplayer, mplayer_inputbox_t* input_box);
void mplayer_inputbox_getsize(mplayer_t* mplayer, mplayer_inputbox_t* input_box);
bool mplayer_inputbox_hover(mplayer_t* mplayer, mplayer_inputbox_t* input_box);
bool mplayer_inputbox_clicked(mplayer_t* mplayer, mplayer_inputbox_t* input_box);
void mplayer_inputbox_renderplaceholder(mplayer_t* mplayer, mplayer_inputbox_t* input_box);
char* mplayer_inputbox_getrenderabledata(mplayer_t* mplayer, mplayer_inputbox_t* input_box);
void mplayer_inputbox_renderinputdata(mplayer_t* mplayer, mplayer_inputbox_t* input_box);
void mplayer_inputbox_rendercursor(mplayer_t* mplayer, mplayer_inputbox_t* input_box);
void mplayer_inputbox_display(mplayer_t* mplayer, mplayer_inputbox_t* input_box);
bool mplayer_inputbox_initialize_input_datalist(mplayer_inputbox_t* input_box);
bool mplayer_inputbox_addinput_data(mplayer_inputbox_t* input_box, const char* input_data);
void mplayer_inputbox_initcursor_range(mplayer_inputbox_t* input_box);
void mplayer_inputbox_addcursor_range(mplayer_inputbox_t* input_box, size_t start_cursor, size_t end_cursor,
    size_t start_byteindex, size_t end_byteindex);
int mplayer_inputbox_getcursoroffsetwidth(mplayer_t* mplayer, mplayer_inputbox_t* input_box);
void mplayer_inputbox_printcursor_ranges(mplayer_t* mplayer, mplayer_inputbox_t* input_box);
void mplayer_inputbox_getcursor_ranges(mplayer_t* mplayer, mplayer_inputbox_t* input_box);
void mplayer_inputbox_destroycursor_ranges(mplayer_inputbox_t* input_box);
void mplayer_inputbox_handleinputs(mplayer_t* mplayer, mplayer_inputbox_t* input_box);
char* mplayer_inputbox_getinputdata(mplayer_inputbox_t input_box, size_t* input_datalen);
void mplayer_inputdata_destroy(input_data_t* input);
void mplayer_inputbox_clear(mplayer_inputbox_t* input_box);
void mplayer_inputbox_destroy(mplayer_inputbox_t* input_box);
#endif