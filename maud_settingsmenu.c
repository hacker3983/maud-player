#include "maud_settingsmenu.h"
#include "maud_colorpicker.h"

void maud_settingmenu(maud_t* maud) {
    maud_settingmenu_navbar_t* navbar = &maud->setting_navbar;
    while(SDL_PollEvent(&maud->e)) {
        if(maud->e.type == SDL_QUIT) {
            maud->quit = true; break;
        } else if(maud->e.type == SDL_MOUSEMOTION) {
            maud->mouse_x = maud->e.motion.x, maud->mouse_y = maud->e.motion.y;
        } else if(maud->e.type == SDL_MOUSEBUTTONDOWN || maud->e.type == SDL_MOUSEBUTTONUP) {
            maud->mouse_x = maud->e.button.x, maud->mouse_y = maud->e.button.y;
            if(maud->e.type == SDL_MOUSEBUTTONUP) {
                maud->mouse_clicked = true;
                maud->mouse_buttondown = false;
            } else {
                maud->mouse_buttondown = true;
            }
        } else if(maud->e.type == SDL_KEYDOWN) {
            maud_colorpicker_handleinputbox_events(maud, &navbar->customize_tab.color_picker);
        } else if(maud->e.type == SDL_TEXTINPUT) {
            maud_colorpicker_handleinputbox_events(maud, &navbar->customize_tab.color_picker);
        }
    }
    maud_getwindow_size(maud);
    maud_set_window_title(maud, SETTING_TITLE);
    maud_set_window_color(maud->renderer, setting_wincolor);
    maud_settingmenu_createnavbar(
        maud,
        (SDL_Color){0x28, 0x00, 0x00, 0xFF},
        (SDL_Color){0x0B, 0x03, 0x2D, 0xFF},
        (SDL_Color){0x0B, 0x03, 0x2D, 0xFF}
    );
    maud_settingmenu_initnavbar_tab(maud);
    maud_settingmenu_handlebackbutton(maud);
    maud_settingmenu_handlecustomize_tab(maud);
    maud_settingmenu_handlemusiclibrary_tab(maud);
    maud_settingmenu_handleabout_tab(maud);
    maud_settingmenu_rendernavbar(maud);
    maud_settingmenu_rendertab_underline(
        maud,
        (SDL_Color){0x00, 0xFF, 0x00, 0xFF}
    );
    maud_settingmenu_displaycurrent_tab(maud);
    maud->mouse_clicked = false;
    maud_setcursor(maud, MAUD_CURSOR_DEFAULT);
    SDL_RenderPresent(maud->renderer);
}

void maud_settingmenu_initnavbar_tab(maud_t* maud) {
    maud_settingmenu_navbar_t* navbar = &maud->setting_navbar;
    if(!navbar->tab_init) {
        navbar->current_tab = MAUD_ABOUT_TAB;
        navbar->tab_init = true;
    }
}

void maud_settingmenu_createnavbar(maud_t* maud, SDL_Color color, SDL_Color customize_tabcolor,
    SDL_Color musiclibrary_tabcolor) {
    maud_settingmenu_navbar_t* navbar = &maud->setting_navbar;
    maud_customizetab_t* customize_tab = &navbar->customize_tab;
    maud_musiclibrarytab_t* musiclibrary_tab = &navbar->musiclibrary_tab;
    SDL_Rect *canvas = &navbar->canvas, *backbtn_canvas = &navbar->backbtn_canvas,
             *information_canvas = &navbar->information_canvas;
    int icon_width = 50, icon_height = 50, icon_padding = 20,
        icon_spacing = 40;
    //printf("icon_spacing = %d\n", icon_spacing);
    canvas->w = icon_width + icon_padding + 40, canvas->h = maud->win_height,
    canvas->x = 0, canvas->y = 0;
    navbar->color = color;
 
    backbtn_canvas->w = icon_width + icon_padding,
    backbtn_canvas->h = icon_height + icon_padding,
    backbtn_canvas->x = canvas->x + (canvas->w - backbtn_canvas->w) / 2,
    backbtn_canvas->y = canvas->y + 20;

    SDL_Rect *customize_canvas = &customize_tab->canvas,
             *customize_iconcanvas = &customize_tab->icon_canvas;
    customize_tab->canvas_color = customize_tabcolor;
    customize_iconcanvas->w = icon_width,
    customize_iconcanvas->h = icon_height,
    customize_canvas->w = customize_iconcanvas->w + icon_padding,
    customize_canvas->h = customize_iconcanvas->h + icon_padding;
    customize_canvas->x = backbtn_canvas->x,
    customize_canvas->y = backbtn_canvas->y + backbtn_canvas->h + icon_spacing;
    customize_iconcanvas->x = customize_canvas->x + (customize_canvas->w - customize_iconcanvas->w) / 2;
    customize_iconcanvas->y = customize_canvas->y + (customize_canvas->h - customize_iconcanvas->h) / 2;

    SDL_Rect *musiclibrary_canvas = &musiclibrary_tab->canvas,
             *musiclibrary_iconcanvas = &musiclibrary_tab->icon_canvas;
    musiclibrary_tab->canvas_color = musiclibrary_tabcolor;
    musiclibrary_iconcanvas->w = icon_width, musiclibrary_iconcanvas->h = icon_height;
    musiclibrary_canvas->w = musiclibrary_iconcanvas->w + icon_padding,
    musiclibrary_canvas->h = musiclibrary_iconcanvas->h + icon_padding;
    musiclibrary_canvas->x = customize_canvas->x,
    musiclibrary_canvas->y = customize_canvas->y + customize_canvas->h + icon_spacing;
    musiclibrary_iconcanvas->x = musiclibrary_canvas->x + (musiclibrary_canvas->w - musiclibrary_iconcanvas->w) / 2;
    musiclibrary_iconcanvas->y = musiclibrary_canvas->y + (musiclibrary_canvas->h - musiclibrary_iconcanvas->h) / 2;
    
    information_canvas->w = icon_width + icon_padding,
    information_canvas->h = icon_height + icon_padding;
    information_canvas->x = musiclibrary_canvas->x,
    information_canvas->y = musiclibrary_canvas->y + musiclibrary_canvas->h + icon_spacing;
}

void maud_settingmenu_rendernavbar(maud_t* maud) {
    maud_settingmenu_navbar_t* navbar = &maud->setting_navbar;
    maud_customizetab_t* customize_tab = &navbar->customize_tab;
    maud_musiclibrarytab_t* musiclibrary_tab = &navbar->musiclibrary_tab;
    SDL_Rect *canvas = &navbar->canvas, *backbtn_canvas = &navbar->backbtn_canvas,
             *information_canvas = &navbar->information_canvas;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(navbar->color));
    SDL_RenderDrawRect(maud->renderer, canvas);
    SDL_RenderFillRect(maud->renderer, canvas);

    SDL_Texture* backbtn_texture = IMG_LoadTexture(maud->renderer, "images/back-button.png");
    SDL_RenderCopy(maud->renderer, backbtn_texture, NULL, backbtn_canvas);
    SDL_DestroyTexture(backbtn_texture);

    SDL_SetRenderDrawColor(maud->renderer, color_toparam(customize_tab->canvas_color));
    SDL_RenderDrawRect(maud->renderer, &customize_tab->canvas);
    SDL_RenderFillRect(maud->renderer, &customize_tab->canvas);
    SDL_Texture* customize_texture = IMG_LoadTexture(maud->renderer, "images/customize.png");
    SDL_RenderCopy(maud->renderer, customize_texture, NULL, &customize_tab->icon_canvas);
    SDL_DestroyTexture(customize_texture);

    SDL_SetRenderDrawColor(maud->renderer, color_toparam(musiclibrary_tab->canvas_color));
    SDL_RenderDrawRect(maud->renderer, &musiclibrary_tab->canvas);
    SDL_RenderFillRect(maud->renderer, &musiclibrary_tab->canvas);
    SDL_Texture* musiclibrary_texture = IMG_LoadTexture(maud->renderer, "images/music library.png");
    SDL_RenderCopy(maud->renderer, musiclibrary_texture, NULL, &musiclibrary_tab->icon_canvas);
    SDL_DestroyTexture(musiclibrary_texture);

    SDL_Texture* information_texture = IMG_LoadTexture(maud->renderer, "images/information.png");
    SDL_RenderCopy(maud->renderer, information_texture, NULL, information_canvas);
    SDL_DestroyTexture(information_texture);
}

void maud_settingmenu_handlebackbutton(maud_t* maud) {
    maud_settingmenu_navbar_t* navbar = &maud->setting_navbar;
    if(maud_rect_hover(maud, navbar->backbtn_canvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            printf("Clicked the back button\n");
            navbar->tab_init = false;
            maud->menu_opt = MAUD_DEFAULT_MENU;
            maud->mouse_clicked = false;
        }
    }
}

void maud_settingmenu_handlecustomize_tab(maud_t* maud) {
    maud_settingmenu_navbar_t* navbar = &maud->setting_navbar;
    if(maud_rect_hover(maud, navbar->customize_tab.canvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            printf("Clicked the Customize tab!\n");
            navbar->current_tab = MAUD_CUSTOMIZE_TAB;
            maud->mouse_clicked = false;
        }
    }
}

void maud_settingmenu_handlemusiclibrary_tab(maud_t* maud) {
    maud_settingmenu_navbar_t* navbar = &maud->setting_navbar;
    if(maud_rect_hover(maud, navbar->musiclibrary_tab.canvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            printf("Clicked the Music Library tab!\n");
            navbar->current_tab = MAUD_MUSICLIBRARY_TAB;
            maud->mouse_clicked = false;
        }
    }
}

void maud_settingmenu_handleabout_tab(maud_t* maud) {
    maud_settingmenu_navbar_t* navbar = &maud->setting_navbar;
    if(maud_rect_hover(maud, navbar->information_canvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            printf("Clicked the About tab!\n");
            navbar->current_tab = MAUD_ABOUT_TAB;
            maud->mouse_clicked = false;
        }
    }
}

void maud_settingmenu_rendertab_underline(maud_t* maud, SDL_Color underline_color) {
    maud_settingmenu_navbar_t* navbar = &maud->setting_navbar;
    maud_customizetab_t* customize_tab = &navbar->customize_tab;
    maud_musiclibrarytab_t* musiclibrary_tab = &navbar->musiclibrary_tab;
    SDL_Rect* about_tab = &navbar->information_canvas;
    SDL_Rect underline_canvas = {
        .x = navbar->canvas.x,
        .w = navbar->canvas.w,
        .h = 10
    };
    switch(navbar->current_tab) {
        case MAUD_ABOUT_TAB:
            underline_canvas.y = about_tab->y + about_tab->h;
            break;
        case MAUD_CUSTOMIZE_TAB:
            underline_canvas.y = customize_tab->canvas.y + customize_tab->canvas.h;
            break;
        case MAUD_MUSICLIBRARY_TAB:
            underline_canvas.y = musiclibrary_tab->canvas.y + musiclibrary_tab->canvas.h;
            break;
    }
    underline_canvas.y += 10;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(underline_color));
    SDL_RenderDrawRect(maud->renderer, &underline_canvas);
    SDL_RenderFillRect(maud->renderer, &underline_canvas);
}

void maud_settingmenu_displaycurrent_tab(maud_t* maud) {
    maud_settingmenu_navbar_t* navbar = &maud->setting_navbar;
    switch(navbar->current_tab) {
        case MAUD_ABOUT_TAB:
            maud_settingmenu_displayabout_tab(maud);
            break;
        case MAUD_CUSTOMIZE_TAB:
            maud_settingmenu_displaycustomize_tab(maud);
            break;
        case MAUD_MUSICLIBRARY_TAB:
            maud_settingmenu_displaymusiclibrary_tab(maud);
            break;
    }
}

void maud_settingmenu_destroy(maud_t* maud) {
    maud_settingmenu_navbar_t* navbar = &maud->setting_navbar;
    maud_colorpicker_destroy(&navbar->customize_tab.color_picker);
}