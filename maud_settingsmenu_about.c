#include "maud_settingsmenu.h"

void maud_settingmenu_displayabout_tab(maud_t* maud) {
    maud_settingmenu_navbar_t* navbar = &maud->setting_navbar;
    text_info_t about_textinfo = {
        .font_size = 36,
        .text = "About",
        .text_canvas = {
            .x = navbar->canvas.x + navbar->canvas.w + 10
        },
        .text_color = {0xFF, 0xFF, 0x00, 0xFF},
    };
    maud_textmanager_sizetext(maud->font, &about_textinfo);
    SDL_Texture* about_texture = maud_textmanager_rendertext(maud, maud->font, &about_textinfo);
    SDL_RenderCopy(maud->renderer, about_texture, NULL, &about_textinfo.text_canvas);
    SDL_DestroyTexture(about_texture);
    int start_y = about_textinfo.text_canvas.y + about_textinfo.text_canvas.h + 20;
    text_info_t maud_info[] = {
        {
            .font_size = 28,
            .text = "Welcome to MAUD (MUSIC AUDIO PLAYER)!",
            .text_color = {0x00, 0xFF, 0x00, 0xff},
        },
        {
            .font_size = 24,
            .text = "Maud is a Music Audio Player Application.",
            .text_color = {0xff, 0xff, 0xff, 0xff}
        },
        {
            .font_size = 20,
            .text = "It is written in the C programming language.",
            .text_color = {0xff, 0xff, 0xff, 0xff}
        },
        {
            .font_size = 20,
            .text = "Author: Catsanddogs just a 17 year old kid!",
            .text_color = {0xff, 0xff, 0xff, 0xff}
        },
        {
            .font_size = 20,
            .text = "Github: https://github.com/hacker3983/maud-player",
            .text_color = {0xff, 0xff, 0xff, 0xff}
        },
        {
            .font_size = 20,
            .text = "Youtube: ThePcExpert - https://www.youtube.com/@thepcexpert9830",
            .text_color = {0xff, 0xff, 0xff, 0xff}
        }
    };
    size_t maud_infocount = sizeof(maud_info) / sizeof(text_info_t);
    int max_width = 0, total_height = 0;
    for(size_t i=0;i<maud_infocount;i++) {
        maud_textmanager_sizetext(maud->font, &maud_info[i]);
        if(maud_info[i].text_canvas.w > max_width) {
            max_width = maud_info[i].text_canvas.w;
        }
        total_height += maud_info[i].text_canvas.h;
        if(i > 0) {
            total_height += 20;
        }
    }
    int remaining_space = maud->win_width - navbar->canvas.w - 10;
    SDL_Rect full_canvas = {
        .x = navbar->canvas.x + navbar->canvas.w + (maud->win_width - max_width - navbar->canvas.w) / 2,
        .y = (maud->win_height - total_height) / 2,
        .w = max_width,
        .h = total_height,
    };
    start_y = full_canvas.y;
    for(size_t i=0;i<maud_infocount;i++) {
        int center_x = full_canvas.x, navbar_endx = navbar->canvas.x + navbar->canvas.w;
        if(center_x < navbar->canvas.x + navbar->canvas.w) {
            full_canvas.x = navbar->canvas.x + navbar->canvas.w + 1;
        }
        center_x = full_canvas.x + (full_canvas.w - maud_info[i].text_canvas.w) / 2;
        if(center_x < navbar_endx) {
            center_x = full_canvas.x;
        }
        maud_info[i].text_canvas.x = center_x,
        maud_info[i].text_canvas.y = start_y;
        SDL_Texture* info_texture = maud_textmanager_rendertext(maud, maud->font,
            &maud_info[i]);
        SDL_RenderCopy(maud->renderer, info_texture, NULL, &maud_info[i].text_canvas);
        start_y += maud_info[i].text_canvas.h + 20;
    }
}