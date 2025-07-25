#ifndef _MAUD_INPUTBOXES_H
#define _MAUD_INPUTBOXES_H
#include "maud_player.h"
#include "maud_inputboxesdef.h"
#include "maud_string.h"

void maud_inputbox_init(maud_inputbox_t* inputbox, TTF_Font* font, int font_size, SDL_Color color,
    const char* placeholder, SDL_Color placeholder_color, SDL_Color cursor_color,
    SDL_Color text_color, int x, int y, int width, int height, int cursor_width,
    int cursor_height);
maud_inputbox_t maud_inputbox_create(TTF_Font* font, int font_size, SDL_Color color,
    const char* placeholder, SDL_Color placeholder_color, SDL_Color cursor_color,
    SDL_Color text_color, int x, int y, int width, int height, int cursor_width,
    int cursor_height);
void maud_inputbox_addinputchar(maud_inputbox_t* inputbox, char* utf8_char);
void maud_inputbox_addinputdata(maud_inputbox_t* inputbox, const char* utf8_string);
void maud_inputbox_addselectionleft(maud_inputbox_t* inputbox);
void maud_inputbox_addselectionright(maud_inputbox_t* inputbox);
void maud_inputbox_addselection(maud_inputbox_t* inputbox, int direction);
void maud_inputbox_selectall(maud_inputbox_t* inputbox);
char* maud_inputbox_getselectiondata(maud_inputbox_t* inputbox);
bool maud_inputbox_deleteselection(maud_inputbox_t* inputbox);
void maud_inputbox_clearselection(maud_inputbox_t* inputbox);
void maud_inputbox_printselection(maud_inputbox_t* inputbox);
void maud_inputbox_cursor_resetblink(maud_inputbox_t* inputbox);
char* maud_inputbox_getinputdata(maud_inputbox_t* inputbox);
void maud_inputbox_getprimaryinputdata(maud_inputbox_t* inputbox);
void maud_inputbox_backspace(maud_inputbox_t* inputbox);
void maud_inputbox_renderplaceholder(maud_t* maud, maud_inputbox_t* inputbox);
void maud_inputbox_rendercharacters(maud_t* maud, maud_inputbox_t* inputbox);
void maud_inputbox_rendercursor(maud_t* maud, maud_inputbox_t* inputbox);
bool maud_inputbox_hover(maud_t* maud, maud_inputbox_t* inputbox);
bool maud_inputbox_clicked(maud_t* maud, maud_inputbox_t* inputbox);
void maud_inputbox_display(maud_t* maud, maud_inputbox_t* inputbox);
void maud_inputbox_handle_events(maud_t* maud, maud_inputbox_t* inputbox);
void maud_inputbox_clear(maud_inputbox_t* inputbox);
void maud_inputbox_destroy(maud_inputbox_t* inputbox);
#endif