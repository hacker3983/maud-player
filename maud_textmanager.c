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

char* mplayer_textmanager_truncateascii(TTF_Font* font, text_info_t* text_info, int maximum_width) {
    char* truncated_asciitext = NULL;
    size_t truncated_length = 0, text_len = strlen(text_info->text);
    int total_width = 0, text_width = 0, dot_length = 3;
    TTF_SetFontSize(font, text_info->font_size);
    for(size_t i=0;i<text_len;i++) {
        TTF_SizeText(font, (char[]){text_info->text[i], 0}, &text_width, NULL);
        total_width += text_width;
        truncated_length++;
        if(total_width > maximum_width) {
            truncated_asciitext = calloc(truncated_length+1, sizeof(char));
            strncpy(truncated_asciitext, text_info->text, truncated_length);
            for(size_t j=0;j<dot_length;j++) {
                truncated_asciitext[(truncated_length - dot_length) + j] = '.';
            }
            break;
        }
    }
    return truncated_asciitext;
}

char* mplayer_textmanager_truncateunicode(TTF_Font* font, text_info_t* text_info, int maximum_width) {
    char* truncated_unicodetext = NULL;
    size_t truncated_length = 0, text_len = strlen(text_info->utext),
           last_3bytes_offset[3][2] = {0}, byte_size = 0,
           last_byteindex = 0;
    int total_width = 0, text_width = 0, dot_length = 3;
    TTF_SetFontSize(font, text_info->font_size);
    printf("Truncate unicode\n");
    for(size_t i=0;i<text_len;i++) {
        char* current_utf8char = mplayer_getutf8_char(text_info->utext, &i, text_len);
        TTF_SizeUTF8(font, current_utf8char, &text_width, NULL);
        byte_size = strlen(current_utf8char);
        total_width += text_width;
        mplayer_concatstr(&truncated_unicodetext, current_utf8char);
        last_3bytes_offset[last_byteindex][0] = truncated_length,
        last_3bytes_offset[last_byteindex][1] = truncated_length + byte_size;
        truncated_length += byte_size;
        free(current_utf8char); current_utf8char = NULL;
        last_byteindex = (last_byteindex + 1) % 3;
        if(total_width > maximum_width) {
            free(current_utf8char); current_utf8char = NULL;
            for(size_t j=0;j<3;j++) {
                truncated_length -= last_3bytes_offset[j][1] - last_3bytes_offset[j][0];
            }
            truncated_unicodetext = (char*)realloc(truncated_unicodetext, truncated_length + 4);
            memset(truncated_unicodetext + truncated_length, 0, 4);
            strcpy(truncated_unicodetext + truncated_length, "...");
            truncated_length += 3;
            break;
        }
    }
    return truncated_unicodetext;
}

char* mplayer_textmanager_truncatetext(TTF_Font* font, text_info_t* text_info, int maximum_width) {
    char* truncated_text = NULL, *target_buff = NULL;
    if(text_info->text) {
        truncated_text = mplayer_textmanager_truncateascii(font, text_info, maximum_width);
    } else if(text_info->utext) {
        truncated_text = mplayer_textmanager_truncateunicode(font, text_info, maximum_width);
    }
    return truncated_text;
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