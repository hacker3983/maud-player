#include "music_textmanager.h"

SDL_Texture* mplayer_textmanager_rendertext(mplayer_t* mplayer, TTF_Font* font, text_info_t* text_info) {
    if(!text_info->text) {
        printf("mplayer_rendertext(): the text given is equal to NULL\n");
        return NULL;
    } 
    SDL_Rect text_canvas = text_info->text_canvas;
    TTF_SetFontSize(font, text_info->font_size);
    if(TTF_SizeText(font, text_info->text, &text_canvas.w, &text_canvas.h)) {
    }
    SDL_Surface* surface = TTF_RenderText_Blended(font, text_info->text, text_info->text_color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(mplayer->renderer, surface);
    SDL_FreeSurface(surface);
    text_info->text_canvas = text_canvas;
    return texture;
}

int mplayer_textmanager_sizetext(TTF_Font* font, text_info_t* utext_info) {
    int ret = 0;
    if((ret = TTF_SetFontSize(font, utext_info->font_size)) < 0) {
        return ret;
    }
    if(utext_info->utext) {
        ret = TTF_SizeUTF8(font, utext_info->utext, &utext_info->text_canvas.w, &utext_info->text_canvas.h);
    } else if(utext_info->text) {
        ret = TTF_SizeText(font, utext_info->text, &utext_info->text_canvas.w, &utext_info->text_canvas.h);
    }
    return ret;
}

SDL_Texture* mplayer_textmanager_renderunicode(mplayer_t* mplayer, TTF_Font* font, text_info_t* utext_info) {
    if(!utext_info->utext) {
        return NULL;
    }
    if(mplayer_textmanager_sizetext(font, utext_info) == -1) {
        printf("mplayer_textmanager_sizetext((): failed because %s\n", SDL_GetError());
        return NULL;
    }
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, utext_info->utext, utext_info->text_color);
    if(surface == NULL) {
        printf("TTF_RenderUTF8_BLENDED(): Failed because %s\n", SDL_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(mplayer->renderer, surface);
    SDL_FreeSurface(surface); surface = NULL;
    return texture;
}


void mplayer_textmanager_centerx(mplayer_t* mplayer, text_info_t* text_info) {
    SDL_Rect text_canvas = text_info->text_canvas;
    text_canvas.x = (WIDTH- text_canvas.w) /2;
    text_info->text_canvas = text_canvas;
}

void mplayer_textmanager_centery(mplayer_t* mplayer, text_info_t* text_info) {
    SDL_Rect text_canvas = text_info->text_canvas;
    text_canvas.y = (WIDTH - text_canvas.h)/2;
    text_info->text_canvas = text_canvas;
}

void mplayer_textmanager_centertext(mplayer_t* mplayer, text_info_t* text_info) {
    SDL_Rect text_canvas = text_info->text_canvas;
    text_canvas.x = (WIDTH - text_canvas.w) / 2, text_canvas.y = (WIDTH - text_canvas.h) / 2;
    text_info->text_canvas = text_canvas;
}