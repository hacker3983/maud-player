#ifndef _MUSIC_TEXTMANAGER_H
#define _MUSIC_TEXTMANAGER_H
#include "music_player.h"
#include "music_string.h"

int mplayer_textmanager_sizetext(TTF_Font* font, text_info_t* utext_info);
SDL_Texture* mplayer_textmanager_rendertext(mplayer_t* mplayer, TTF_Font* font, text_info_t* text_info);
SDL_Texture* mplayer_textmanager_renderunicode(mplayer_t* mplayer, TTF_Font* font, text_info_t* utext_info);
void mplayer_textmanager_centerx(mplayer_t* mplayer, text_info_t* text_info);
void mplayer_textmanager_centery(mplayer_t* mplayer, text_info_t* text_info);
void mplayer_textmanager_centertext(mplayer_t* mplayer, text_info_t* text_info);
#endif