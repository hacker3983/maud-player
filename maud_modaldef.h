#ifndef _MAUD_MODALDEF_H
#define _MAUD_MODALDEF_H
#include "maud_textinfodef.h"
#include "maud_inputboxesdef.h"

typedef struct maud_modal_header {
    int padding_x, padding_y,
        padding_height;
    SDL_Rect canvas; // canvas for the header canvas
    bool show_bgcolor; // a flag to determine wether to render the background color
    SDL_Color color; // background color for the header
    text_info_t title; // the title information for the header
} maud_modal_header_t;

typedef struct maud_modal_image {
    int padding_y;
    SDL_Rect canvas; // The canvas for the modal image
    bool show_bgcolor; // whether to render the background color for the image
    SDL_Color color; // background color for the image
    char* path;
    SDL_Texture* texture; // texture of the image
} maud_modal_image_t;

typedef struct maud_modal_button {
    int padding_x, padding_y,
        padding_width, padding_height;
    SDL_Rect canvas;
    SDL_Color color;
    text_info_t text;
    bool clicked;
} maud_modal_button_t;

typedef struct maud_modal_buttoncontainer {
    int padding_y,
        padding_height;
    SDL_Rect canvas;
    SDL_Color color;
} maud_modal_buttoncontainer_t;

typedef struct maud_modal_inputbox {
    int padding_y;
    maud_inputbox_t inputbox;
} maud_modal_input_t;

typedef struct maud_modal {
    TTF_Font* font;
    SDL_Rect canvas;
    SDL_Color color;
    maud_modal_header_t header;
    maud_modal_image_t image;
    int inputbox_padding;
    maud_modal_input_t input;
    maud_modal_button_t confirm_button;
    maud_modal_button_t cancel_button;
    maud_modal_buttoncontainer_t button_container;
    bool enable_buttoncontainer, leave;
} maud_modal_t;
#endif