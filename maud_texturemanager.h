#ifndef _MAUD_TEXTUREMANAGER_H
#define _MAUD_TEXTUREMANAGER_H
#include "maud_player.h"

SDL_Texture** maud_texturemanager_createtexture_list(size_t amount);
void maud_texturemanager_realloctexture(SDL_Texture*** texture_objs, size_t* amount);
void maud_texturemanager_destroytextures(SDL_Texture** textures, size_t n);
#endif