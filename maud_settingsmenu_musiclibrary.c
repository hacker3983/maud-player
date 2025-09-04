#include "maud_settingsmenu.h"

void maud_settingmenu_rendermusiclibrary_headertext(maud_t* maud,
    maud_musiclibrarytab_t* musiclibrary_tab) {
    maud_settingmenu_navbar_t* navbar = &maud->setting_navbar;
    text_info_t header_text = {
        .font_size = 36,
        .text = "Music Library",
        .text_canvas = {
            .x = navbar->canvas.x + navbar->canvas.w + 10
        },
        .text_color = {0xFF, 0xFF, 0x00, 0xFF},
    };
    maud_textmanager_sizetext(maud->font, &header_text);
    musiclibrary_tab->header_text = header_text;
    SDL_Texture* headertext_texture = maud_textmanager_rendertext(maud, maud->font,
        &header_text);
    SDL_RenderCopy(maud->renderer, headertext_texture, NULL,
        &header_text.text_canvas);
    SDL_DestroyTexture(headertext_texture);
}

void maud_settingmenu_rendermusiclibrary_locationsection(maud_t* maud,
    maud_musiclibrarytab_t* musiclibrary_tab) {
    maud_settingmenu_navbar_t* navbar = &maud->setting_navbar;
    text_info_t header_text = musiclibrary_tab->header_text;
    text_info_t location_text = {
        .font_size = 30,
        .text = "Locations: ",
        .text_color = {0xFF, 0xFF, 0xFF, 0xFF},
        .text_canvas = {
            .x = navbar->canvas.x + navbar->canvas.w + 10,
            .y = header_text.text_canvas.y + header_text.text_canvas.h + 20
        }
    };
    maud_textmanager_sizetext(maud->font, &location_text);
    SDL_Texture* locationtext_texture = maud_textmanager_rendertext(maud, maud->font,
        &location_text);
    SDL_RenderCopy(maud->renderer, locationtext_texture, NULL,
        &location_text.text_canvas);
    SDL_DestroyTexture(locationtext_texture);
    musiclibrary_tab->location_text = location_text;
    maud_settingmenu_rendermusiclibrary_addlocationbtn(maud, musiclibrary_tab);
    maud_settingmenu_rendermusiclibrary_locations(maud, musiclibrary_tab);
}

void maud_settingmenu_rendermusiclibrary_addlocationbtn(maud_t* maud, maud_musiclibrarytab_t* musiclibrary_tab) {
    text_info_t location_text = musiclibrary_tab->location_text;
    text_info_t addlocation_text = {
        .font_size = 30,
        .text = " Add Location",
        .text_color = {0x00, 0xFF, 0x00, 0xFF},
        .text_canvas = {0},
    };
    maud_textmanager_sizetext(maud->font, &addlocation_text);
    int icon_width = addlocation_text.text_canvas.h, icon_height = addlocation_text.text_canvas.h,
        total_width = addlocation_text.text_canvas.w + icon_width;
    maud_addlocationbtn_t addlocation_btn = {
        .color = {0x0A, 0x2E, 0x36, 0xFF}/*{0x0B, 0x03, 0x2D, 0xFF}*/,
        .text = addlocation_text,
        .canvas = {
            .w = total_width + 10,
            .h = addlocation_text.text_canvas.h + 10
        },
        .icon_canvas = {
            .w = icon_width,
            .h = icon_height
        }
    };
    SDL_Rect* icon_canvas = &addlocation_btn.icon_canvas;
    addlocation_btn.canvas.x = maud->win_width - addlocation_btn.canvas.w - 10;
    addlocation_btn.canvas.y = location_text.text_canvas.y;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(addlocation_btn.color));
    SDL_RenderDrawRect(maud->renderer, &addlocation_btn.canvas);
    SDL_RenderFillRect(maud->renderer, &addlocation_btn.canvas);

    icon_canvas->x = addlocation_btn.canvas.x + (addlocation_btn.canvas.w -
        total_width) / 2;
    icon_canvas->y = addlocation_btn.canvas.y + (addlocation_btn.canvas.h -
        icon_canvas->h) / 2;
    SDL_Texture* icon_texture = IMG_LoadTexture(maud->renderer, "images/add-folder.png");
    SDL_RenderCopy(maud->renderer, icon_texture, NULL, icon_canvas);
    SDL_DestroyTexture(icon_texture);

    addlocation_text.text_canvas.x = icon_canvas->x + icon_canvas->w;
    addlocation_text.text_canvas.y = addlocation_btn.canvas.y + (addlocation_btn.canvas.h -
        addlocation_text.text_canvas.h) / 2;
    SDL_Texture* addlocation_texture = maud_textmanager_rendertext(maud, maud->font, &addlocation_text);
    SDL_RenderCopy(maud->renderer, addlocation_texture, NULL, &addlocation_text.text_canvas);
    SDL_DestroyTexture(addlocation_texture);
    musiclibrary_tab->addlocation_btn = addlocation_btn;
}

void maud_settingmenu_rendermusiclibrary_locations(maud_t* maud,
    maud_musiclibrarytab_t* musiclibrary_tab) {
    maud_settingmenu_navbar_t* navbar = &maud->setting_navbar;
    text_info_t header_text = musiclibrary_tab->header_text;
    int start_y = musiclibrary_tab->addlocation_btn.canvas.y +
        musiclibrary_tab->addlocation_btn.canvas.h + 10;
    SDL_Rect outer_canvas = {
        .x = header_text.text_canvas.x,
        .y = start_y,
        .w = maud->win_width - navbar->canvas.w - 10
    },
    removebtn_canvas = {
        .w = 25, .h = 25
    };
    text_info_t music_location = {
        .font_size = 28,
        .text_color = {0xFF, 0xFF, 0xFF, 0xFF},
        .text_canvas = {
            .x = outer_canvas.x + 10
        }
    };
    SDL_Color outer_canvascolor = {0x7B, 0x08, 0x28, 0xFF}
    /*{0x03, 0x47, 0x48, 0xFF}{0x31, 0x85, 0xFC, 0xFF}{0xC4, 0x20, 0x21, 0xFF}{0x0B, 0x03, 0x2D, 0xFF}*/;
    for(size_t i=0;i<maud->location_count;i++) {
        char* current_location = NULL;
        #ifdef _WIN32
        music_location.utext = maud_widetoutf8(maud->locations[i].path);
        #else
        music_location.utext = maud->locations[i].path;
        #endif
        maud_textmanager_sizetext(maud->music_font, &music_location);
        outer_canvas.h = music_location.text_canvas.h + 20;
        SDL_SetRenderDrawColor(maud->renderer, color_toparam(outer_canvascolor));
        SDL_RenderDrawRect(maud->renderer, &outer_canvas);
        SDL_RenderFillRect(maud->renderer, &outer_canvas);
        music_location.text_canvas.y = outer_canvas.y + (outer_canvas.h -
            music_location.text_canvas.h) / 2;

        SDL_Texture* music_locationtexture = maud_textmanager_renderunicode(maud,
            maud->music_font, &music_location);
        SDL_RenderCopy(maud->renderer, music_locationtexture, NULL,
            &music_location.text_canvas);
        SDL_DestroyTexture(music_locationtexture);

        removebtn_canvas.x = outer_canvas.x + (outer_canvas.w - removebtn_canvas.w - 10);
        removebtn_canvas.y = outer_canvas.y + (outer_canvas.h - removebtn_canvas.h) / 2;
        SDL_Texture* removebtn_texture = IMG_LoadTexture(maud->renderer,
            "images/removelocation.png");
        SDL_RenderCopy(maud->renderer, removebtn_texture, NULL, &removebtn_canvas);
        SDL_DestroyTexture(removebtn_texture);
        if(maud_rect_hover(maud, removebtn_canvas) && maud->mouse_clicked) {
            maud_filemanager_delmusic_locationindex(maud, i);
            maud->mouse_clicked = false;
        }
        #ifdef _WIN32
        free(music_location.utext);
        #endif
        outer_canvas.y += outer_canvas.h + 10;
    }
}


void maud_settingmenu_displaymusiclibrary_tab(maud_t* maud) {
    maud_settingmenu_navbar_t* navbar = &maud->setting_navbar;
    maud_musiclibrarytab_t* musiclibrary_tab = &navbar->musiclibrary_tab;
    maud_addlocationbtn_t* addlocation_btn = &musiclibrary_tab->addlocation_btn;
    maud_settingmenu_rendermusiclibrary_headertext(maud, musiclibrary_tab);
    maud_settingmenu_rendermusiclibrary_locationsection(maud, musiclibrary_tab);
}