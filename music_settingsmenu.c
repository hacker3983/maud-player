#include "music_settingsmenu.h"

void mplayer_settingmenu(mplayer_t* mplayer) {
    mplayer_settingmenu_navbar_t navbar = {0};
    mplayer_settingmenu_librarycategory_t music_librarycategory = {0};
    mplayer_settingmenu_personalizationcategory_t personalization_category = {0};
    while(SDL_PollEvent(&mplayer->e)) {
        if(mplayer->e.type == SDL_QUIT) {
            mplayer->quit = true; break;
        } else if(mplayer->e.type == SDL_MOUSEMOTION) {
            mplayer->mouse_x = mplayer->e.motion.x, mplayer->mouse_y = mplayer->e.motion.y;
        } else if(mplayer->e.type == SDL_MOUSEBUTTONDOWN) {
            mplayer->mouse_x = mplayer->e.button.x, mplayer->mouse_y = mplayer->e.button.y;
            mplayer->mouse_clicked = true;
        } else if(mplayer->e.type == SDL_MOUSEWHEEL) {
            mplayer_scrolltype_getmousewheel_scrolltype(mplayer->e, &mplayer->scroll_type);
            mplayer->scroll = true;
        }
    }
    mplayer_set_window_title(mplayer, SETTING_TITLE);
    mplayer_set_window_color(mplayer->renderer, setting_wincolor);
    SDL_GetWindowSize(mplayer->window, &WIDTH, &HEIGHT);
    if(mplayer->scroll) {
        mplayer_scrollcontainer_performscroll(mplayer, &mplayer->settingmenu_scrollcontainer);
        mplayer->scroll = false;
    }

    // Create the navigation bar and handle its components
    mplayer_settingmenu_create_navigationbar(mplayer, &navbar);
    if(mplayer_settingmenu_handle_backbtn(mplayer, &navbar)) {
        return;
    }

    mplayer->settingmenu_scrollcontainer_index = 0;
    if(!mplayer->settingmenu_scrollcontainer.init) {
        SDL_Rect scroll_area = {
            .x = 0, .y = 0,
            .w = WIDTH, .h = HEIGHT
        };
        mplayer_scrollcontainer_init(&mplayer->settingmenu_scrollcontainer, scroll_area, SETTING_LINESPACING * 2);
    }

    mplayer_settingmenu_render_musiclibrary(mplayer, navbar, &music_librarycategory);
    //mplayer_settingmenu_render_personalization(mplayer, music_librarycategory.text_info.text_canvas,
    //    &personalization_category);
    //mplayer_settingmenu_render_about(mplayer, personalization_category);

    // Render the navigation bar
    mplayer_settingmenu_render_navigationbar(mplayer, &navbar);
    if(!mplayer->settingmenu_scrollcontainer_init) {
        mplayer->settingmenu_scrollcontainer_init = true;
    }
    mplayer->mouse_clicked = false;
    // Activate the cursor that was set if none was set then we will just use the default cursor
    mplayer_activatecursor(mplayer);
    mplayer_setcursor(mplayer, MPLAYER_CURSOR_DEFAULT);
    SDL_RenderPresent(mplayer->renderer);
}

void mplayer_settingmenu_create_navigationbar(mplayer_t* mplayer, mplayer_settingmenu_navbar_t* navbar_ref) {
    mplayer_textmanager_sizetext(mplayer->font, &setting_textinfo[0]);
    int max_navbarheight = (setting_btns[0].btn_canvas.h > setting_textinfo[0].text_canvas.h) ?
            setting_btns[0].btn_canvas.h : setting_textinfo[0].text_canvas.h;

    navbar_ref->color = (SDL_Color){0x39, 0x2F, 0x5A, 0xFF}/*{0x5E, 0xB1, 0xBF, 0xFF}{0x04, 0x2A, 0x2B, 0xFF}{0x00, 0xA6, 0x76, 0xff}*/;
    navbar_ref->canvas.x = 0, navbar_ref->canvas.y = 0;
    navbar_ref->canvas.w = WIDTH, navbar_ref->canvas.h = max_navbarheight;

    // Center the settings button vertically on the navigation bar
    setting_btns[0].btn_canvas.y = navbar_ref->canvas.y + (navbar_ref->canvas.h - setting_btns[0].btn_canvas.h)/2;

    // Set "Settings" text position to come after the back button icons and center it vertically
    setting_textinfo[0].text_canvas.x = setting_btns[0].btn_canvas.x + setting_btns[0].btn_canvas.w + 10;
    setting_textinfo[0].text_canvas.y = navbar_ref->canvas.y + (navbar_ref->canvas.h -
        setting_textinfo[0].text_canvas.h)/2;

    navbar_ref->backbtn_canvas = setting_btns[0].btn_canvas;
    navbar_ref->backbtn_textinfo = setting_textinfo[0];

    mplayer_tooltip_t backbtn_tooltip = {
        .font = mplayer->music_font,
        .font_size = 20,
        .text = setting_btns[0].tooltip_text,
        .x = setting_btns[0].btn_canvas.x + setting_btns[0].btn_canvas.w,
        .y = 0,
        .margin_x = 10,
        .margin_y = 10,
        .background_color = {0xC1, 0x62, 0x00, 0xFF},
        .delay_secs = 0,
        .duration_secs = 0,
        .element_canvas = navbar_ref->backbtn_canvas,
        .wrap_length = WIDTH,
        .wrap_spacing = 5
    };
    navbar_ref->backbtn_tooltip = backbtn_tooltip;
}

bool mplayer_settingmenu_handle_backbtn(mplayer_t* mplayer, mplayer_settingmenu_navbar_t* navbar_ref) {
    if(mplayer_rect_hover(mplayer, navbar_ref->backbtn_canvas)) {
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
        if(mplayer->mouse_clicked) {
            mplayer->menu_opt = MPLAYER_DEFAULT_MENU;
            mplayer_menumanager_setup_menu(mplayer);
            mplayer->mouse_clicked = false;
            return true;
        }
    }
    return false;
}

void mplayer_settingmenu_render_navigationbar(mplayer_t* mplayer, mplayer_settingmenu_navbar_t* navbar_ref) {
    SDL_Texture* backbtn_texture = mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][setting_btns[0].texture_idx];
    SDL_Texture* backbtn_text_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font, &setting_textinfo[0]);

    // Render the background for the navigation bar
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(navbar_ref->color));
    SDL_RenderDrawRect(mplayer->renderer, &navbar_ref->canvas);
    SDL_RenderFillRect(mplayer->renderer, &navbar_ref->canvas);

    // Render the contents of the navigation bar onto it's background
    SDL_RenderCopy(mplayer->renderer, backbtn_texture, NULL, &navbar_ref->backbtn_canvas);
    SDL_RenderCopy(mplayer->renderer, backbtn_text_texture, NULL, &navbar_ref->backbtn_textinfo.text_canvas);
    mplayer_tooltip_render(mplayer, &navbar_ref->backbtn_tooltip);
}


void mplayer_settingmenu_render_musiclibrary(mplayer_t* mplayer, mplayer_settingmenu_navbar_t navbar,
    mplayer_settingmenu_librarycategory_t* library_category) {    
    library_category->text_info = setting_textinfo[1];
    if(!mplayer->settingmenu_scrollcontainer_init) {
        library_category->text_info.text_canvas.y = navbar.canvas.y + navbar.canvas.h + SETTING_LINESPACING;
        mplayer_textmanager_sizetext(mplayer->font, &library_category->text_info);
        music_scrollcontaineritem_t scroll_item = {
            .disappear_y = -setting_textinfo[1].text_canvas.h,
            .initial_canvas = library_category->text_info.text_canvas,
            .vertical_scroll = true
        };
        mplayer_scrollcontainer_appenditem(&mplayer->settingmenu_scrollcontainer, scroll_item);
    }
    // Render music library text
    if(mplayer->settingmenu_scrollcontainer.items[mplayer->settingmenu_scrollcontainer_index].render) {
        SDL_Texture* library_text_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font,
            &library_category->text_info);
        SDL_RenderCopy(mplayer->renderer, library_text_texture, NULL,
            &mplayer->settingmenu_scrollcontainer.items[mplayer->settingmenu_scrollcontainer_index].scroll_canvas);
        SDL_DestroyTexture(library_text_texture);
    }   
    mplayer->settingmenu_scrollcontainer_index++;
    // Render the music locations that contain the musics
    mplayer_settingmenu_render_musiclocations(mplayer, library_category->text_info.text_canvas);
}

void mplayer_settingmenu_render_musiclocations(mplayer_t* mplayer, SDL_Rect previous_canvas) {
    text_info_t music_location = {
        .font_size = 20,
        .text = NULL,
        .text_canvas = {
            .x = 10, .y = previous_canvas.y + previous_canvas.h + SETTING_LINESPACING,
            .w = 0, .h = 0
        },
        .text_color = {0xFF, 0xFF, 0xFF, 0xFF},
        .utext = NULL
    };
    for(size_t i=0;i<mplayer->location_count;i++) {
        if(!mplayer->locations[i].path) {
            return;
        }
        music_location.utext = mplayer_widetoutf8(mplayer->locations[i].path);
        mplayer_textmanager_sizetext(mplayer->font, &music_location);
        if(!mplayer->settingmenu_scrollcontainer_init) {
            music_scrollcontaineritem_t scroll_item = {
                .disappear_y = -music_location.text_canvas.h,
                .initial_canvas = music_location.text_canvas,
                .vertical_scroll = true
            };
            mplayer_scrollcontainer_appenditem(&mplayer->settingmenu_scrollcontainer, scroll_item);
        }
        if(mplayer->settingmenu_scrollcontainer.items[mplayer->settingmenu_scrollcontainer_index].render) {
            SDL_Texture* location_texture = mplayer_textmanager_renderunicode(mplayer, mplayer->font, &music_location);
            SDL_RenderCopy(mplayer->renderer, location_texture, NULL,
                &mplayer->settingmenu_scrollcontainer.items[mplayer->settingmenu_scrollcontainer_index].scroll_canvas);
            SDL_DestroyTexture(location_texture); location_texture = NULL;
        }
        music_location.text_canvas.y += music_location.text_canvas.h + SETTING_LINESPACING;
        mplayer->settingmenu_scrollcontainer_index++;
        free(music_location.utext); music_location.utext = NULL;
    }
}

void mplayer_settingmenu_render_personalization(mplayer_t* mplayer, SDL_Rect previous_canvas,
    mplayer_settingmenu_personalizationcategory_t* personalization_category) {
    personalization_category->text_info = setting_textinfo[2];
    personalization_category->text_info.text_canvas.y = previous_canvas.y + previous_canvas.h + SETTING_LINESPACING;
    SDL_Texture* personalization_text_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font,
        &personalization_category->text_info);
    SDL_RenderCopy(mplayer->renderer, personalization_text_texture, NULL,
        &personalization_category->text_info.text_canvas);
    SDL_DestroyTexture(personalization_text_texture); personalization_text_texture = NULL;
}
void mplayer_settingmenu_render_about(mplayer_t* mplayer,
    mplayer_settingmenu_personalizationcategory_t personalization_category) {
    setting_textinfo[3].text_canvas.y = personalization_category.text_info.text_canvas.y +
        personalization_category.text_info.text_canvas.h + SETTING_LINESPACING;
    mplayer_settingmenu_aboutcategory_t about = {
        .text_info = setting_textinfo[3]
    };
    SDL_Texture* about_text_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font, &about.text_info);
    SDL_RenderCopy(mplayer->renderer, about_text_texture, NULL, &about.text_info.text_canvas);
    SDL_DestroyTexture(about_text_texture); about_text_texture = NULL;
}