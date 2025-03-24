#ifndef _MUSIC_INPUTBOXESDEF_H
#define _MUSIC_INPUTBOXESDEF_H

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
    size_t start_renderpos,
           end_renderpos;
    bool update_renderpos;

    SDL_Color input_datacolor;
    char* placeholder;
    SDL_Rect placeholder_canvas;
    SDL_Color placeholder_color;

    SDL_Rect inputbox_canvas;
    SDL_Color inputbox_color;

    SDL_Rect cursor_canvas;
    SDL_Color cursor_color;
    bool hover, clicked;
} mplayer_inputbox_t;
#endif