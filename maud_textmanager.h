#ifndef _MAUD_TEXTMANAGER_H
#define _MAUD_TEXTMANAGER_H
#include "maud_player.h"
#include "maud_string.h"

int maud_textmanager_sizetext(TTF_Font* font, text_info_t* utext_info);
SDL_Texture* maud_textmanager_rendertext(maud_t* maud, TTF_Font* font, text_info_t* text_info);
SDL_Texture* maud_textmanager_renderunicode(maud_t* maud, TTF_Font* font, text_info_t* utext_info);
char* maud_textmanager_truncateascii(TTF_Font* font, text_info_t* text_info, int maximum_width);
char* maud_textmanager_truncateunicode(TTF_Font* font, text_info_t* text_info, int maximum_width);
char* maud_textmanager_truncatetext(TTF_Font* font, text_info_t* text_info, int maximum_width);
void maud_textmanager_centerx(maud_t* maud, text_info_t* text_info);
void maud_textmanager_centery(maud_t* maud, text_info_t* text_info);
void maud_textmanager_centertext(maud_t* maud, text_info_t* text_info);
#endif