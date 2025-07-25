#include "maud_tooltips.h"
#include "maud_texturemanager.h"
#include "maud_string.h"
#include "maud_textmanager.h"

void maud_tooltip_getsize(maud_tooltip_t* tooltip) {
    int text_w = 0, text_h = 0;
    size_t tooltip_textlen = strlen(tooltip->text);
    TTF_SetFontSize(tooltip->font, tooltip->font_size);
    int text_portionw = 0, text_portionh = 0;
    char* utf8_char = NULL, *text_portion = NULL;
    int utf8_charw = 0, utf8_charh = 0,
        total_wrapspacing = 0;
    for(size_t i=0;i<tooltip_textlen;i++) {
        utf8_char = maud_getutf8_char(tooltip->text, &i, tooltip_textlen);
        TTF_SizeUTF8(tooltip->font, utf8_char, &utf8_charw, &utf8_charh);
        if(text_portionw >= tooltip->wrap_length) {
            if(text_portionw > text_w) {
                text_w = text_portionw;
            }
            text_h += text_portionh;
            total_wrapspacing += tooltip->wrap_spacing;
            text_portionw = 0, text_portionh = 0;
        }
        if(utf8_charh > text_portionh) {
            text_portionh = utf8_charh;
        }
        text_portionw += utf8_charw;
        free(utf8_char); utf8_char = NULL;
    }
    if(text_portionw > text_w) {
        text_w = text_portionw;
    }
    text_h += text_portionh;
    tooltip->w = text_w + (tooltip->margin_x * 2), tooltip->h = text_h + (tooltip->margin_y * 2) + total_wrapspacing;
}

void maud_tooltip_getcontents(maud_tooltip_t* tooltip, SDL_Rect tooltip_canvas) {
    text_info_t* contents = NULL, content = {
        .font_size = tooltip->font_size,
        .text = NULL,
        .text_canvas = {
            .x = tooltip_canvas.x + tooltip->margin_x,
            .y = tooltip_canvas.y + tooltip->margin_y,
        },
        .text_color = tooltip->text_color,
        .utext = NULL
    };
    size_t content_count = 0, text_len = strlen(tooltip->text);
    char* utf8_char = NULL, *text_portion = NULL;
    int utf8_charw = 0, utf8_charh = 0,
        text_portionwidth = 0, text_portionheight = 0;
    TTF_SetFontSize(tooltip->font, tooltip->font_size);
    for(size_t i=0;i<text_len;i++) {
        utf8_char = maud_getutf8_char(tooltip->text, &i, text_len);
        TTF_SizeUTF8(tooltip->font, utf8_char, &utf8_charw, &utf8_charh);
        if(text_portionwidth + utf8_charw >= tooltip->wrap_length) {
            if(!contents) {
                contents = malloc(sizeof(text_info_t));            
            } else {
                contents = realloc(contents, (content_count + 1) * sizeof(text_info_t));
            }
            content.utext = text_portion;
            maud_textmanager_sizetext(tooltip->font, &content);
            contents[content_count++] = content;
            content.text_canvas.y += content.text_canvas.h + tooltip->wrap_spacing;
            text_portion = NULL,
            text_portionwidth = 0,
            text_portionheight = 0;
        }
        maud_concatstr(&text_portion, utf8_char);
        text_portionwidth += utf8_charw;
        free(utf8_char); utf8_char = NULL;
    }
    if(text_portion) {
        if(!contents) {
           contents = malloc(sizeof(text_info_t));            
        } else {
            contents = realloc(contents, (content_count + 1) * sizeof(text_info_t));
        }
        content.utext = text_portion;
        maud_textmanager_sizetext(tooltip->font, &content);
        contents[content_count++] = content;
    }
    tooltip->contents = contents;
    tooltip->content_count = content_count;
}

void maud_tooltip_render(maud_t* maud, maud_tooltip_t* tooltip) {
    maud_tooltip_getsize(tooltip);
    SDL_Rect tooltip_canvas = {
        tooltip->x, // The horizontal position for the tooltip
        tooltip->y, // The vertical position for the tooltip
        tooltip->w, // Width of the tooltip
        tooltip->h  // Height of the tooltip
    };
    // Get the width and the height of the text so we can determine the tooltip box dimensions
    TTF_SetFontSize(tooltip->font, tooltip->font_size);
    // TODO: IMPLEMENT BALLOON TOOLTIP and also the delay functionality
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(tooltip->background_color));
    SDL_RenderDrawRect(maud->renderer, &tooltip_canvas);
    SDL_RenderFillRect(maud->renderer, &tooltip_canvas);

    maud_tooltip_getcontents(tooltip, tooltip_canvas);
    maud_tooltip_displaycontents(maud, tooltip);
    maud_tooltip_printcontents(tooltip);
    maud_tooltip_destroycontents(tooltip);
}

void maud_tooltip_renderhover(maud_t* maud, maud_tooltip_t* tooltip) {
    // Ensure that the tooltip pops up only when the user hovers of the particular elements canvas such as a button
    // or icon on screen
    if(!maud_rect_hover(maud, tooltip->element_canvas)) {
        return;
    }
    maud_tooltip_render(maud, tooltip);
}

void maud_tooltip_displaycontents(maud_t* maud, maud_tooltip_t* tooltip) {
    for(size_t i=0;i<tooltip->content_count;i++) {
        SDL_Texture* content_texture = maud_textmanager_renderunicode(maud, tooltip->font, &tooltip->contents[i]);
        SDL_RenderCopy(maud->renderer, content_texture, NULL, &tooltip->contents[i].text_canvas);
        SDL_DestroyTexture(content_texture);
    }
}

void maud_tooltip_printcontents(maud_tooltip_t* tooltip) {
    for(size_t i=0;i<tooltip->content_count;i++) {
        printf("tooltip->contents[%zu]: %s\n", i, tooltip->contents[i].utext);
    }
}

void maud_tooltip_destroycontents(maud_tooltip_t* tooltip) {
    for(size_t i=0;i<tooltip->content_count;i++) {
        free(tooltip->contents[i].utext);
    }
    free(tooltip->contents);
    tooltip->contents = NULL;
    tooltip->content_count = 0;
}