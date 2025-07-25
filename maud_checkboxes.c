#include "maud_checkboxes.h"

bool maud_checkbox_hovered(maud_t* maud) {
    if((maud->mouse_x <= checkbox_size.x + checkbox_size.w && maud->mouse_x >= checkbox_size.x) &&
        (maud->mouse_y <= checkbox_size.y + checkbox_size.h && maud->mouse_y >= checkbox_size.y)) {
            return true;
    }
    return false;
}

void maud_drawmusic_checkbox(maud_t* maud, SDL_Color box_color, SDL_Color fill_color,
    bool fill, SDL_Color tick_color, bool check) {
    mcheckbox_t checkbox_info = {0};
    checkbox_info.checkbox_canvas = checkbox_size;
    checkbox_info.box_color = box_color;
    checkbox_info.tk_color = tick_color;
    checkbox_info.fill_color = fill_color;
    checkbox_info.tick = check;
    checkbox_info.fill = fill;
    maud_drawcheckbox(maud, &checkbox_info);
}

void maud_drawcheckbox(maud_t* maud, mcheckbox_t* checkbox_info) {
    int mid_x1 = checkbox_info->checkbox_canvas.x,
        mid_y1 = checkbox_info->checkbox_canvas.y,
        mid_x2 = checkbox_info->checkbox_canvas.x + checkbox_info->checkbox_canvas.w,
        mid_y2 = checkbox_info->checkbox_canvas.y + checkbox_info->checkbox_canvas.h;
    SDL_Color box_color = checkbox_info->box_color, tick_color = checkbox_info->tk_color,
        fill_color = checkbox_info->fill_color;
    SDL_SetRenderDrawColor(maud->renderer, box_color.r, box_color.g, box_color.b, box_color.a);
    SDL_RenderDrawRect(maud->renderer, &checkbox_info->checkbox_canvas);
    if(checkbox_info->fill) {
        SDL_SetRenderDrawColor(maud->renderer, fill_color.r, fill_color.g, fill_color.b, fill_color.a);
        SDL_RenderFillRect(maud->renderer, &checkbox_info->checkbox_canvas);
    }
    if(checkbox_info->tick) {
        SDL_SetRenderDrawColor(maud->renderer, tick_color.r, tick_color.g, tick_color.b, tick_color.a);
        // draw longest line for tick
        int y = 10, y2 = 6;
        for(int i=0;i<2;i++) {
            int k = mid_x1+10;
            for(int j=mid_y2-y;j>=mid_y1+5;j--) {
                SDL_RenderDrawPoint(maud->renderer, k, j);
                k++;
            }
            y--;
        }
        y = 10;
        for(int i=0;i<2;i++) {
            int k = mid_x1+10;
            for(int j=mid_y2-y;j>=(mid_y2-y2)-y;j--) {
                SDL_RenderDrawPoint(maud->renderer, k, j);
                k--;
            }
            y--; y2--;
        }
    }
}