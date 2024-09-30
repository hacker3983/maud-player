#include "music_tooltips.h"

void mplayer_tooltip_render(mplayer_t* mplayer, mplayer_tooltip_t* tooltip) {
    // Ensure that the tooltip pops up only when the user hovers of the particular elements canvas such as a button
    // or icon on screen
    if(!mplayer_rect_hover(mplayer, tooltip->element_canvas)) {
        return;
    }
    SDL_Rect tooltip_canvas = {
        tooltip->element_canvas.x, // The horizontal position for the tooltip
        tooltip->element_canvas.y, // The vertical position for the tooltip
        tooltip->margin_x * 2, // Width of the tooltip
        tooltip->margin_y * 2  // Height of the tooltip
    };
    // Get the width and the height of the text so we can determine the tooltip box dimensions
    TTF_SetFontSize(tooltip->font, tooltip->font_size);
    int tooltip_text_w = 0, tooltip_text_h = 0;
    TTF_SizeText(tooltip->font, tooltip->text, &tooltip_text_w, &tooltip_text_h);
    tooltip_canvas.w += tooltip_text_w;
    tooltip_canvas.h += tooltip_text_h;

    if(tooltip->x < 0) {
        tooltip_canvas.x -= tooltip_canvas.w + (-tooltip->x);
    } else {
        tooltip_canvas.x += tooltip->x + tooltip->element_canvas.w;
    }

    if(tooltip->y < 0) {
        tooltip_canvas.y -= tooltip_canvas.h + (-tooltip->y);
    } else if(tooltip->y > 0) {
        tooltip_canvas.y -= tooltip_canvas.h + tooltip->y;
    }

    // TODO: IMPLEMENT BALLOON TOOLTIP and also the delay functionality
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(tooltip->background_color));
    SDL_RenderDrawRect(mplayer->renderer, &tooltip_canvas);
    SDL_RenderFillRect(mplayer->renderer, &tooltip_canvas);

    SDL_Rect tooltip_textcanvas = {
        tooltip_canvas.x + tooltip->margin_x,
        tooltip_canvas.y + tooltip->margin_y,
        tooltip_text_w,
        tooltip_text_h
    };

    SDL_Surface* tooltip_surface = TTF_RenderText_Blended(tooltip->font, tooltip->text, tooltip->text_color);
    SDL_Texture* tooltip_texture = SDL_CreateTextureFromSurface(mplayer->renderer, tooltip_surface);
    SDL_RenderCopy(mplayer->renderer, tooltip_texture, NULL, &tooltip_textcanvas);
    SDL_FreeSurface(tooltip_surface); tooltip_surface = NULL;
    SDL_DestroyTexture(tooltip_texture); tooltip_texture = NULL;
}