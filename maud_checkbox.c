#include "maud_checkbox.h"

void maud_checkbox_drawtick_shortline(maud_t* maud, maud_checkbox_t* checkbox) {
    maud_checkbox_tick_t* tick = &checkbox->tick_line;
    int percentage_x1 = checkbox->canvas.w * 10 / 100,
	    percentage_y1 = checkbox->canvas.h * 50 / 100,
	    percentage_y2 = checkbox->canvas.h * 70 / 100;
    maud_checkbox_tickline_t shortline = {
        .x1 = checkbox->canvas.x + percentage_x1,
        .y1 = checkbox->canvas.y + percentage_y1,
        .y2 = checkbox->canvas.y + percentage_y2
    };
    while(shortline.y1 <= shortline.y2) {
        SDL_RenderDrawPoint(maud->renderer, shortline.x1, shortline.y1);
        shortline.x1++; shortline.y1++;
    }
    tick->shortline = shortline;
}

void maud_checkbox_drawtick_longline(maud_t* maud, maud_checkbox_t* checkbox) {
    maud_checkbox_tick_t* tick = &checkbox->tick_line;
    maud_checkbox_tickline_t* shortline = &tick->shortline;
	int percentage_x2 = checkbox->canvas.w * 90 / 100,
	    percentage_y2 = checkbox->canvas.h * 5 / 100,
	    x2 = checkbox->canvas.x + percentage_x2;
    maud_checkbox_tickline_t longline = {
        .x1 = shortline->x1,
        .x2 = checkbox->canvas.x + percentage_x2,
        .y1 = shortline->y1,
        .y2 = checkbox->canvas.y + percentage_y2
    };
    while(longline.x1 <= longline.x2) {
        if(longline.y1 >= longline.y2) {
            SDL_RenderDrawPoint(maud->renderer, longline.x1, longline.y1);
            longline.y1--;
        } else {
            break;
        }
        longline.x1++;
    }
    tick->longline = longline;
}

void maud_checkbox_drawtick(maud_t* maud, maud_checkbox_t* checkbox) {
    if(!checkbox->tick) {
        return;
    }
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(checkbox->tick_color));
    maud_checkbox_drawtick_shortline(maud, checkbox);
    maud_checkbox_drawtick_longline(maud, checkbox);
}

void maud_checkbox_draw(maud_t* maud, maud_checkbox_t* checkbox) {
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(checkbox->color));
    SDL_RenderDrawRect(maud->renderer, &checkbox->canvas);
    if(checkbox->fill) {
        SDL_SetRenderDrawColor(maud->renderer, color_toparam(checkbox->fill_color));
        SDL_RenderFillRect(maud->renderer, &checkbox->canvas);
    }
    maud_checkbox_drawtick(maud, checkbox);
}