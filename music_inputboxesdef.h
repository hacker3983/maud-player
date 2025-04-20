#ifndef _MUSIC_INPUTBOXESDEF_H
#define _MUSIC_INPUTBOXESDEF_H
#include "music_textinfodef.h"

enum music_inputbox_datatype {
    MPLAYER_INPUTBOXDATA_ASCII,
    MPLAYER_INPUTBOXDATA_OTHER
};

typedef struct input_data {
    char* data; // The full utf8 input string
    size_t datasize; // The total number of bytes in the input_data
    size_t *byte_positions; // The starting and ending position as a pair for each utf8 character in input_data string
    size_t cursor_pos; // The current cursor position
    size_t datalen; // The total number of characters in the utf8 input_data string
    size_t byte_positionlen; // The total length of byte positions
} input_data_t;

typedef struct music_inputbox {
    TTF_Font *placeholder_font, *input_datafont;
    input_data_t input;
    text_info_t input_text;
    char* renderable_data;
    size_t* cursor_ranges;
    size_t cursor_rangecount;
    bool update_renderpos;

    SDL_Color input_datacolor;
    char* placeholder, *placeholder_truncated;
    text_info_t placeholder_info;
    SDL_Color placeholder_color;
    bool render_placeholder;

    SDL_Rect inputbox_canvas, temp_canvas;
    SDL_Color inputbox_color;
    bool inputbox_fill;

    SDL_Rect cursor_canvas;
    SDL_Color cursor_color;
    double blink_timeoutsecs;
    uint64_t blink_timeout;
    bool show_cursor, cursor_blink;
    bool hover, clicked;
} mplayer_inputbox_t;
#endif