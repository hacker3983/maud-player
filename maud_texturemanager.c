#include "music_texturemanager.h"


SDL_Texture** mplayer_texturemanager_createtexture_list(size_t amount) {
    SDL_Texture** objtexture_list = calloc(amount, sizeof(SDL_Texture*));
    return objtexture_list;
}
void mplayer_texturemanager_realloctexture(SDL_Texture*** texture_objs, size_t* amount) {
    (*amount)++;
    (*texture_objs) = realloc(*texture_objs, (*amount) * sizeof(SDL_Texture*));
    (*texture_objs)[(*amount)-1] = NULL;
}

void mplayer_texturemanager_destroytextures(SDL_Texture** textures, size_t n) {
    if(textures == NULL) {
        return;
    }
    for(size_t i=0;i<n;i++) {
        if(textures[i] != NULL) {
            SDL_DestroyTexture(textures[i]);
            textures[i] = NULL;
        }
    }
}