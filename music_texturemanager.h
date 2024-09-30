#ifndef _MUSIC_TEXTUREMANAGER_H
#define _MUSIC_TEXTUREMANAGER_H
#include "music_player.h"

SDL_Texture** mplayer_texturemanager_createtexture_list(size_t amount);
void mplayer_texturemanager_realloctexture(SDL_Texture*** texture_objs, size_t* amount);
void mplayer_texturemanager_destroytextures(SDL_Texture** textures, size_t n);
#endif