#include "music_settingsmenu.h"

void mplayer_settingmenu(mplayer_t* mplayer) {
    mplayer_scrollcontainer_t scroll_container = {0};
    SDL_Rect* canvas = NULL,
              /* The current element canvas will be used to identify each element on the screen
                 for example buttons, images, text, etc. This information will be used by the scroll container
                 in order to manipulate the canvas positions for each element on the screen
              */
              current_element_canvas = {0};
    size_t *scrollcontainer_index = &mplayer->settingmenu_scrollcontainer_index;
    int btn_id = 0,
        disappear_x = 0, disappear_y = 0;
    bool mouse_clicked = false;
    while(SDL_PollEvent(&mplayer->e)) {
        if(mplayer->e.type == SDL_QUIT) {
            mplayer->quit = 1;
            break;
        } else if(mplayer->e.type == SDL_WINDOWEVENT && mplayer->e.window.event == SDL_WINDOWEVENT_RESIZED) {
            mplayer->window_resized = true;
        } else if(mplayer->e.type == SDL_MOUSEMOTION) {
            mplayer->mouse_x = mplayer->e.motion.x, mplayer->mouse_y = mplayer->e.motion.y;
            if(mplayer_buttonmanager_ibuttons_hover(mplayer, setting_btns, &btn_id, SETTINGSBTN_COUNT)) {
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
            } else {
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_DEFAULT);
            }
        } else if(mplayer->e.type == SDL_MOUSEBUTTONUP) {
            mplayer->mouse_x = mplayer->e.button.x, mplayer->mouse_y = mplayer->e.button.y;
            if(mplayer_buttonmanager_ibuttons_hover(mplayer, setting_btns, &btn_id, SETTINGSBTN_COUNT)) {
                if(btn_id == BACK_BUTTON) {
                    mplayer->menu_opt = MPLAYER_DEFAULT_MENU;
                    mplayer_menumanager_setup_menu(mplayer);
                    mplayer_setcursor(mplayer, MPLAYER_CURSOR_DEFAULT);
                    return;
                }
            }
            mouse_clicked = true, mplayer->mouse_clicked = true;
        } else if(mplayer->e.type == SDL_MOUSEWHEEL) {
            mplayer_scrolltype_getmousewheel_scrolltype(mplayer->e, &mplayer->scroll_type);
            mplayer->scroll = true;
        }
    }
    mplayer_set_window_title(mplayer, SETTING_TITLE);
    mplayer_set_window_color(mplayer->renderer, setting_wincolor);
    SDL_GetWindowSize(mplayer->window, &WIDTH, &HEIGHT);
    mplayer_displayprogression_control(mplayer);
    // create buttons text, normal and display on the screen
    for(size_t i=0;i<setting_textinfo_size;i++) {
        mplayer->menu->textures[MPLAYER_TEXT_TEXTURE][i] = mplayer_textmanager_rendertext(mplayer, mplayer->font, &setting_textinfo[i]);
    }

    // set the canvas x position of the text "Go Back To Home" beside the back button 
    canvas = &setting_textinfo[0].text_canvas;
    canvas->x = setting_btns[0].btn_canvas.x + setting_btns[0].btn_canvas.w + 5;

    SDL_Rect navbar_canvas = {0}, bg_canvas;
    SDL_Color navbar_canvascolor = (SDL_Color){0x83, 0x22, 0x32, 0xFF}/*black(SDL_Color){0x75, 0x0D, 0x37, 0xFF}*/,
              bg_canvascolor = {0};
    navbar_canvas.w = WIDTH,
    navbar_canvas.h = setting_btns[0].btn_canvas.y + setting_btns[0].btn_canvas.h + SETTING_LINESPACING;
    if(mplayer->scroll && mplayer->settingmenu_scrollcontainer_init) {
        mplayer_scrollcontainer_areainfo_t scrollarea_info = {
            .mouse_x = mplayer->mouse_x,
            .mouse_y = mplayer->mouse_y,
            .scroll_area = {.x = 0, .y = navbar_canvas.y + navbar_canvas.h, .w = WIDTH,
                    .h = HEIGHT - navbar_canvas.h - navbar_canvas.y}
        };
        mplayer_scrollcontainers_performscroll(mplayer->settingmenu_scrollcontainers, mplayer->scroll_type,
            SETTING_LINESPACING * 3, mplayer->settingmenu_scrollcontainer_count, scrollarea_info);
        mplayer->scroll = false;
    }
    // Set the vertical scroll minimum height to stop rendering content on the screen
    disappear_y = navbar_canvas.y;
    
    // tooltip for back button
    mplayer_tooltip_t backbtn_tooltip = {
        .background_color = {0x1D, 0x26, 0x3B, 0xFF},
        .text_color = {0x30, 0x66, 0xBE, 0xFF},
        .text = setting_btns[0].tooltip_text,
        .x = 1,
        .y = 0,
        .margin_x = 10,
        .margin_y = 10,
        .delay_secs = 0,
        .duration_secs = 0,
        .element_canvas = setting_btns[0].btn_canvas,
        .font = mplayer->music_font,
        .font_size = 18,
    };

    canvas->y = roundf((float)(navbar_canvas.h - canvas->h) / (float)2);
    setting_btns[0].btn_canvas.y = roundf((float)(navbar_canvas.h - setting_btns[0].btn_canvas.h) / (float)2);
   
    for(size_t i=1;i<setting_textinfo_size;i++) {
        setting_textinfo[i].text_canvas.y = setting_textinfo[i-1].text_canvas.y +
                        setting_textinfo[i-1].text_canvas.h + SETTING_LINESPACING;
    }
    // Draw a background for the Music Location category
    canvas = &setting_textinfo[1].text_canvas;
    bg_canvascolor = black /*(SDL_Color){0x5A, 0x9D, 0xA6, 0xFF}*/;
    bg_canvas = (SDL_Rect){0};
    bg_canvas.x = setting_textinfo[1].text_canvas.x - 5, bg_canvas.y = canvas->y;
    bg_canvas.w = setting_textinfo[1].text_canvas.w + 10/*WIDTH*/, bg_canvas.h = canvas->h + SETTING_LINESPACING;
    canvas->x = 20;
    if(mplayer->settingmenu_scrollcontainer_init) {
        bg_canvas.y = mplayer->settingmenu_scrollcontainers[*scrollcontainer_index].scroll_canvas.y;
    }
    canvas->y = bg_canvas.y + roundf((float)(bg_canvas.h - setting_textinfo[1].text_canvas.h) / (float)2);
    if(mplayer->settingmenu_scrollcontainer_init) {
        if(mplayer->settingmenu_scrollcontainers[*scrollcontainer_index].render) {
            SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(bg_canvascolor));
            SDL_RenderDrawRect(mplayer->renderer, &bg_canvas);
            SDL_RenderFillRect(mplayer->renderer, &bg_canvas);
        }
    }

    // set the current element canvas to equal to the Music Location category of elements
    current_element_canvas.x = bg_canvas.x, current_element_canvas.y = bg_canvas.y;
    current_element_canvas.w = bg_canvas.w,
    current_element_canvas.h = bg_canvas.h; // Set the maximum height the current elements on screen

    // Draw a canvas to hold the button in and set the canvas position for the add folder button
    canvas = &setting_btns[1].btn_canvas;
    canvas->x = WIDTH - canvas->w;
    canvas->y = bg_canvas.y + roundf((float)(bg_canvas.h - canvas->h)/(float)2);

    setting_textinfo[2].text_canvas.x = canvas->x - setting_textinfo[2].text_canvas.w - 10;
    setting_textinfo[2].text_canvas.y = bg_canvas.y + roundf((float)(bg_canvas.h - setting_textinfo[2].text_canvas.h)
        / (float)2);

    bg_canvascolor = black/*(SDL_Color){0x81, 0x17, 0x1B, 0xFF}*/;
    bg_canvas.w = canvas->w + setting_textinfo[2].text_canvas.w + 10, bg_canvas.h = canvas->h;
    bg_canvas.x = setting_textinfo[2].text_canvas.x - 5;
    if(!mplayer->settingmenu_scrollcontainer_init) {
        bg_canvas.y = setting_textinfo[1].text_canvas.y + roundf((float)(bg_canvas.h - (canvas->h - SETTING_LINESPACING))
        / (float)2);
    } else {
        bg_canvas.y = mplayer->settingmenu_scrollcontainers[(*scrollcontainer_index)++].scroll_canvas.y;
    }
    // Get the width in pixel in between the Add Folder button and Music Location background text
    // This will give us the total width of each elements on screen so we can perform the scrolling operations on them
    current_element_canvas.w += bg_canvas.x - current_element_canvas.w + bg_canvas.w;

    // Finally we use this calculation and append it to the scroll container
    scroll_container = mplayer_scrollcontainer_create(current_element_canvas, disappear_y,
        true, true);
    if(!mplayer->settingmenu_scrollcontainer_init) {
        printf("Scroll container was created for Music Location Category:\n");
        mplayer_scrollcontainer_printinfo(scroll_container);
    }
    mplayer_settingmenu_tryappend_scrollcontainer(mplayer, scroll_container);

    if(mplayer_rect_hover(mplayer, bg_canvas)) {
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
        if(mouse_clicked) {
            size_t prev_location_count = mplayer->location_count;
            mplayer_filemanager_browsefolder(mplayer);
            if(prev_location_count != mplayer->location_count) {
                mplayer_settingmenu_scrollcontainer_update(mplayer);
                return;
            }
        }
    }

    if(mplayer->settingmenu_scrollcontainer_init) {
        if(mplayer->settingmenu_scrollcontainers[0].render) {
            SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(bg_canvascolor));
            SDL_RenderDrawRect(mplayer->renderer, &bg_canvas);
            SDL_RenderFillRect(mplayer->renderer, &bg_canvas);
        }
    }

    // Render each music location from the music paths info file
    mplayer_settingmenu_render_musiclocations(mplayer, canvas, disappear_y);

    // copy each texture on its particular canvas
    for(size_t i=1;i<setting_textinfo_size;i++) {
        mplayer->menu->texture_canvases[MPLAYER_TEXT_TEXTURE][i] = setting_textinfo[i].text_canvas;
        SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_TEXT_TEXTURE][i], NULL,
            &mplayer->menu->texture_canvases[MPLAYER_TEXT_TEXTURE][i]);
    }

    // render buttons on screen
    mplayer_settingmenu_render_settingbtns(mplayer);
    mplayer_settingmenu_render_navigationbar(mplayer, &navbar_canvas, navbar_canvascolor);
    
    // Render tooltip onto the screen whenever the user hovers over the back button    
    mplayer_tooltip_render(mplayer, &backbtn_tooltip);

    if(!mplayer->settingmenu_scrollcontainer_init) {
        mplayer->settingmenu_scrollcontainer_init = true;
    }
    SDL_RenderPresent(mplayer->renderer);
    mplayer_texturemanager_destroytextures(mplayer->menu->textures[MPLAYER_TEXT_TEXTURE],
        mplayer->menu->texture_sizes[MPLAYER_TEXT_TEXTURE]);
    mplayer_menumanager_menu_freetext(mplayer, MPLAYER_SETTINGS_MENU);
    mplayer->settingmenu_scrollcontainer_index = 0;
}

void mplayer_settingmenu_render_navigationbar(mplayer_t* mplayer, SDL_Rect* navbar_canvas, SDL_Color navbar_canvascolor) {
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(navbar_canvascolor));
    SDL_RenderDrawRect(mplayer->renderer, navbar_canvas);
    SDL_RenderFillRect(mplayer->renderer, navbar_canvas);

    mplayer->menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][0] = setting_btns[0].btn_canvas;
    SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][0], NULL,
        &mplayer->menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][0]);

    mplayer->menu->texture_canvases[MPLAYER_TEXT_TEXTURE][0] = setting_textinfo[0].text_canvas;
    SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_TEXT_TEXTURE][0], NULL,
        &mplayer->menu->texture_canvases[MPLAYER_TEXT_TEXTURE][0]);
}

void mplayer_settingmenu_render_settingbtns(mplayer_t* mplayer) {
    for(size_t i=1;i<SETTINGSBTN_COUNT;i++) {
        mplayer->menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][i] = setting_btns[i].btn_canvas;
        SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][i], NULL,
            &setting_btns[i].btn_canvas);
    }
}

void mplayer_settingmenu_render_musiclocations(mplayer_t* mplayer, SDL_Rect* previous_canvas, int disappear_y) {
    bool *mouse_clicked = &mplayer->mouse_clicked;
    mplayer_scrollcontainer_t scroll_container = {0};
    text_info_t music_location = {24,
        NULL,
        NULL,
        {0xE9, 0x76, 0x5B, 0xFF}/*white*/,
        {0}
    };
    SDL_Color bg_canvascolor = {0x3C, 0x2C, 0x44, 0xFF};
    SDL_Rect bg_canvas = {
        .x = 0, .y = music_location.text_canvas.y
    },
    *canvas = {0},
    current_element_canvas = {0};

    music_location.text_canvas.x = 50;
    music_location.text_canvas.y = previous_canvas->y + previous_canvas->h + SETTING_LINESPACING;
    bg_canvas.x = 0, bg_canvas.y = music_location.text_canvas.y;
    bg_canvascolor = (SDL_Color){0x3C, 0x2C, 0x44, 0xFF};

    /*(SDL_Color){0x0F, 0x52, 0x57, 0xFF}(SDL_Color){0x81, 0x17, 0x1B, 0xFF}(SDL_Color){0x1E, 0x96, 0xFC, 0xFF}{0x58, 0x72, 0x91, 0xFF}*/;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(bg_canvascolor));
    for(size_t i=0;i<mplayer->location_count;i++) {
        /*if(!mplayer->locations[i].render) {
            continue;
        }*/
        #ifdef _WIN32
        music_location.utext = mplayer_widetoutf8(mplayer->locations[i].path); 
        #else
        music_location.utext = mplayer->locations[i].path;
        #endif
        mplayer_textmanager_sizetext(mplayer->music_font, &music_location);
        bg_canvas.w = WIDTH, bg_canvas.h = music_location.text_canvas.h + SETTING_LINESPACING;
        if(mplayer->settingmenu_scrollcontainer_init) {
            bg_canvas.y = mplayer->settingmenu_scrollcontainers[i+1].scroll_canvas.y;
        }
        music_location.text_canvas.y = bg_canvas.y + roundf((float)(bg_canvas.h - music_location.text_canvas.h) / (float)2);
        canvas = &music_removebtn.btn_canvas;
        canvas->x = WIDTH - (canvas->w * 2);
        canvas->y = music_location.text_canvas.y;

        current_element_canvas = bg_canvas;
        if(!mplayer->settingmenu_scrollcontainer_init) {
            scroll_container = mplayer_scrollcontainer_create(current_element_canvas, disappear_y, true, true);
            printf("Scroll container has been created for the music location %ls:\n",
                mplayer->locations[i].path);
            mplayer_scrollcontainer_printinfo(scroll_container);
        }
        mplayer_settingmenu_tryappend_scrollcontainer(mplayer, scroll_container);
        if(mplayer_buttonmanager_ibutton_hover(mplayer, music_removebtn)) {
            mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
            if(*mouse_clicked) {
                printf("removing music location %ls along with related musics\n", mplayer->locations[i].path);
                mplayer_filemanager_delmusic_locationindex(mplayer, i);;
                *mouse_clicked = false;
                continue;
            }
        }
        if(mplayer->settingmenu_scrollcontainers[i+1].render) {
            SDL_Texture* texture = mplayer_textmanager_renderunicode(mplayer, mplayer->music_font, &music_location);
            SDL_RenderDrawRect(mplayer->renderer, &bg_canvas);
            SDL_RenderFillRect(mplayer->renderer, &bg_canvas);
            mplayer_menumanager_addmenu_texture(mplayer, MPLAYER_TEXT_TEXTURE);
            mplayer_menumanager_menuplace_texture(mplayer, MPLAYER_TEXT_TEXTURE, texture, music_location.text_canvas);
            bg_canvas.y = bg_canvas.y + bg_canvas.h + SETTING_LINESPACING;
            SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][music_removebtn.texture_idx],
                NULL, canvas);
            SDL_RenderCopy(mplayer->renderer,
                mplayer->menu->textures[MPLAYER_TEXT_TEXTURE][mplayer->menu->texture_sizes[MPLAYER_TEXT_TEXTURE]-1], NULL,
                &mplayer->menu->texture_canvases[MPLAYER_TEXT_TEXTURE][mplayer->menu->texture_sizes[MPLAYER_TEXT_TEXTURE]
                - 1]);
        }
    }
}

void mplayer_settingmenu_tryappend_scrollcontainer(mplayer_t* mplayer, mplayer_scrollcontainer_t container) {
    if(mplayer->settingmenu_scrollcontainer_init) {
        return;
    }
    mplayer_scrollcontainer_append(&mplayer->settingmenu_scrollcontainers, container, &mplayer->settingmenu_scrollcontainer_count);
}

void mplayer_settingmenu_scrollcontainer_update(mplayer_t* mplayer) {
    mplayer_scrollcontainer_destroylist(&mplayer->settingmenu_scrollcontainers,
    &mplayer->settingmenu_scrollcontainer_count);
    mplayer->settingmenu_scrollcontainer_init = false;
    printf("mplayer->settingmenu_scrollcontainer was updated\n");
}