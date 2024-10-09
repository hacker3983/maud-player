#ifndef _MUSIC_INPUTBOXESDEF_H
#define _MUSIC_INPUTBOXESDEF_H

enum music_inputbox_datatype {
    MPLAYER_INPUTBOXDATA_ASCII,
    MPLAYER_INPUTBOXDATA_OTHER
};

typedef struct music_inputdata {
    char* data;
    int datatype;
    size_t cursor_pos, datalen;
} mplayer_inputdata_t;

typedef struct music_inputbox {
    TTF_Font *placeholder_font, *input_datafont;
    mplayer_inputdata_t* input_datalist, *parsed_input_datalist;
    size_t input_datacount, parsed_datacount, input_datacursor,
        start_renderindex[2];
    SDL_Color input_datacolor;
    char* placeholder;
    SDL_Rect placeholder_canvas;
    SDL_Color placeholder_color;

    SDL_Rect inputbox_canvas;
    SDL_Color inputbox_color;

    SDL_Rect cursor_canvas;
    SDL_Color cursor_color;
    char* text_portion;
    bool hover, clicked, calculate_renderpos;
} mplayer_inputbox_t;
#endif