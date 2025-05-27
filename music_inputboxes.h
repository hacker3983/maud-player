#ifndef _MUSIC_INPUTBOXES_H
#define _MUSIC_INPUTBOXES_H
#include "music_player.h"
#include "music_inputboxesdef.h"
#include "music_string.h"

void mplayer_inputbox_init(mplayer_inputbox_t* inputbox, TTF_Font* font, int font_size, SDL_Color color,
    const char* placeholder, SDL_Color placeholder_color, SDL_Color cursor_color,
    SDL_Color text_color, int x, int y, int width, int height, int cursor_width,
    int cursor_height);
mplayer_inputbox_t mplayer_inputbox_create(TTF_Font* font, int font_size, SDL_Color color,
    const char* placeholder, SDL_Color placeholder_color, SDL_Color cursor_color,
    SDL_Color text_color, int x, int y, int width, int height, int cursor_width,
    int cursor_height);
void mplayer_inputbox_addinputchar(mplayer_inputbox_t* inputbox, char* utf8_char);
void mplayer_inputbox_addinputdata(mplayer_inputbox_t* inputbox, const char* utf8_string);
void mplayer_inputbox_addselectionleft(mplayer_inputbox_t* inputbox);
void mplayer_inputbox_addselectionright(mplayer_inputbox_t* inputbox);
void mplayer_inputbox_addselection(mplayer_inputbox_t* inputbox, int direction);
void mplayer_inputbox_selectall(mplayer_inputbox_t* inputbox);
char* mplayer_inputbox_getselectiondata(mplayer_inputbox_t* inputbox);
void mplayer_inputbox_printselection(mplayer_inputbox_t* inputbox);
void mplayer_inputbox_cursor_resetblink(mplayer_inputbox_t* inputbox);
char* mplayer_inputbox_getinputdata(mplayer_inputbox_t* inputbox);
void mplayer_inputbox_getprimaryinputdata(mplayer_inputbox_t* inputbox);
void mplayer_inputbox_backspace(mplayer_inputbox_t* inputbox);
void mplayer_inputbox_renderplaceholder(mplayer_t* mplayer, mplayer_inputbox_t* inputbox);
void mplayer_inputbox_rendercharacters(mplayer_t* mplayer, mplayer_inputbox_t* inputbox);
void mplayer_inputbox_rendercursor(mplayer_t* mplayer, mplayer_inputbox_t* inputbox);
bool mplayer_inputbox_hover(mplayer_t* mplayer, mplayer_inputbox_t* inputbox);
bool mplayer_inputbox_clicked(mplayer_t* mplayer, mplayer_inputbox_t* inputbox);
void mplayer_inputbox_display(mplayer_t* mplayer, mplayer_inputbox_t* inputbox);
void mplayer_inputbox_handle_events(mplayer_t* mplayer, mplayer_inputbox_t* inputbox);
void mplayer_inputbox_clear(mplayer_inputbox_t* inputbox);
void mplayer_inputbox_destroy(mplayer_inputbox_t* inputbox);
#endif