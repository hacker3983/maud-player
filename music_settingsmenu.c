#include "music_settingsmenu.h"

void mplayer_settingmenu(mplayer_t* mplayer) {
    music_itemcontainer_t* settingmenu_itemcontainer = &mplayer->settingmenu_itemcontainer;
    music_scrollcontainer_t* settingmenu_scrollcontainer = &mplayer->settingmenu_scrollcontainer;
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
    SDL_GetWindowSize(mplayer->window, &mplayer->win_width, &mplayer->win_height);
    // Create the navigation bar and handle its components
    mplayer_settingmenu_create_navigationbar(mplayer, &navbar);
    if(mplayer_settingmenu_handle_backbtn(mplayer, &navbar)) {
        mplayer_itemcontainer_destroy(&mplayer->settingmenu_itemcontainer);
        mplayer_scrollcontainer_destroy(&mplayer->settingmenu_scrollcontainer);
        return;
    }
    SDL_Rect scroll_area = {
        .x = 0, .y = navbar.canvas.y + navbar.canvas.h,
        .w = mplayer->win_width, .h = mplayer->win_height - (navbar.canvas.y + navbar.canvas.h)
    };
    mplayer_scrollcontainer_setscroll_area(settingmenu_scrollcontainer, scroll_area);
    mplayer_settingmenu_render_musiclibrary(mplayer, navbar, &music_librarycategory);
    mplayer_settingmenu_render_personalization(mplayer, settingmenu_itemcontainer->items[settingmenu_itemcontainer->item_index-1],
        &personalization_category);
    mplayer_settingmenu_render_about(mplayer, personalization_category);

    mplayer_scrollcontainer_setprops(&mplayer->settingmenu_scrollcontainer, scroll_area, 20, mplayer->settingmenu_contentcount);
    // Render the navigation bar
    if(mplayer->scroll) {
        printf("Scrolling\n");
        mplayer_scrollcontainer_performscroll_overscrollarea(mplayer, settingmenu_scrollcontainer);
        printf("settingmenu_itemcount: %zu, 2: %zu, scroll: %zu\n", settingmenu_itemcontainer->item_count, mplayer->settingmenu_contentcount,
            settingmenu_scrollcontainer->item_container.item_count);
        mplayer->scroll = false;
    }
    mplayer_settingmenu_handleupdate(mplayer);
    mplayer_itemcontainer_resetitem_index(settingmenu_itemcontainer);
    mplayer_scrollcontainer_resetitem_index(settingmenu_scrollcontainer);
    mplayer->settingmenu_contentcount = 0;
    mplayer_settingmenu_render_navigationbar(mplayer, &navbar);
    mplayer->mouse_clicked = false;
    mplayer_setcursor(mplayer, MPLAYER_CURSOR_DEFAULT);
    SDL_RenderPresent(mplayer->renderer);
}

void mplayer_settingmenu_handleupdate(mplayer_t* mplayer) {
    if(mplayer->settingmenu_scrollcontainer.item_container.items) {
        //mplayer_itemcontainer_set_hasallitems_status(&mplayer->settingmenu_scrollcontainer.item_container, true);
        mplayer_scrollcontainer_init(&mplayer->settingmenu_scrollcontainer);
    }
    if(mplayer->settingmenu_itemcontainer.item_count != mplayer->settingmenu_contentcount) {
        mplayer->settingmenu_itemcontainer.has_allitems = false;
    } else {
        mplayer_itemcontainer_set_hasallitems_status(&mplayer->settingmenu_itemcontainer, true);
    }
}

void mplayer_settingmenu_additem(mplayer_t* mplayer, SDL_Rect canvas) {
    music_itemcontainer_t *settingmenu_itemcontainer = &mplayer->settingmenu_itemcontainer;
    if(settingmenu_itemcontainer->has_allitems) {
        //printf("has_allitems: true\n");
        mplayer_itemcontainer_setnextitem_canvas(settingmenu_itemcontainer, canvas);
        return;
    }
    if((!settingmenu_itemcontainer->items) ||
        settingmenu_itemcontainer->item_count < mplayer->settingmenu_contentcount) {
        mplayer_itemcontainer_additems(settingmenu_itemcontainer, 1);
    }
    mplayer_itemcontainer_setnextitem_canvas(settingmenu_itemcontainer, canvas);
}

void mplayer_settingmenu_create_navigationbar(mplayer_t* mplayer, mplayer_settingmenu_navbar_t* navbar_ref) {
    mplayer_textmanager_sizetext(mplayer->font, &setting_textinfo[0]);
    int max_navbarheight = (setting_btns[0].btn_canvas.h > setting_textinfo[0].text_canvas.h) ?
            setting_btns[0].btn_canvas.h : setting_textinfo[0].text_canvas.h;

    navbar_ref->color = (SDL_Color){0x39, 0x2F, 0x5A, 0xFF}/*{0x5E, 0xB1, 0xBF, 0xFF}{0x04, 0x2A, 0x2B, 0xFF}{0x00, 0xA6, 0x76, 0xff}*/;
    navbar_ref->canvas.x = 0, navbar_ref->canvas.y = 0;
    navbar_ref->canvas.w = mplayer->win_width, navbar_ref->canvas.h = max_navbarheight;

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
        .wrap_length = mplayer->win_width,
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
    mplayer_tooltip_renderhover(mplayer, &navbar_ref->backbtn_tooltip);
}

void mplayer_settingmenu_render_musiclibrary(mplayer_t* mplayer, mplayer_settingmenu_navbar_t navbar,
    mplayer_settingmenu_librarycategory_t* library_category) {    
    library_category->text_info = setting_textinfo[1];
    mplayer_textmanager_sizetext(mplayer->font, &library_category->text_info);
    library_category->text_info.text_canvas.y = mplayer->settingmenu_scrollcontainer.scroll_y;
    mplayer->settingmenu_contentcount++;
    if(mplayer->settingmenu_itemcontainer.item_index == mplayer->settingmenu_scrollcontainer.content_renderpos) {
        if(library_category->text_info.text_canvas.y <
            mplayer->settingmenu_scrollcontainer.scroll_area.y + mplayer->settingmenu_scrollcontainer.scroll_area.h) {
                mplayer_scrollcontainer_additem(&mplayer->settingmenu_scrollcontainer, library_category->text_info.text_canvas);
                //printf("Increasing item count to %zu\n", mplayer->settingmenu_scrollcontainer.item_container.item_count);
                mplayer_scrollcontainer_setnext_itemcanvas(&mplayer->settingmenu_scrollcontainer, library_category->text_info.text_canvas);
                //printf("Added item to scroll container and item_count is %zu\n", mplayer->settingmenu_scrollcontainer.item_container.item_count);
        }
        // Render music library text
        SDL_Texture* library_text_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font,
            &library_category->text_info);
        SDL_RenderCopy(mplayer->renderer, library_text_texture, NULL, &library_category->text_info.text_canvas);
        SDL_DestroyTexture(library_text_texture);
    }
    mplayer_settingmenu_additem(mplayer, library_category->text_info.text_canvas);

    // Render the music locations that contain the musics
    mplayer_settingmenu_render_musiclocations(mplayer, library_category->text_info.text_canvas);
}

void mplayer_settingmenu_render_musiclocations(mplayer_t* mplayer, SDL_Rect previous_canvas) {
    music_scrollcontainer_t* settingmenu_scrollcontainer = &mplayer->settingmenu_scrollcontainer;
    music_itemcontainer_t* settingmenu_itemcontainer = &mplayer->settingmenu_itemcontainer;
    text_info_t music_location = {
        .font_size = 30,
        .text = NULL,
        .text_canvas = {
            .x = 10, .y = previous_canvas.y + previous_canvas.h + SETTING_LINESPACING,
            .w = 0, .h = 0
        },
        .text_color = {0xFF, 0xFF, 0xFF, 0xFF},
        .utext = NULL
    };
    size_t iteration_count = 0;
    for(size_t i=0;i<mplayer->location_count;i++) {
        if(!mplayer->locations[i].path) {
            return;
        }
        //printf("mplayer->locations[%zu].path = %ls\n", i, mplayer->locations[i].path);
        #ifdef _WIN32
        music_location.utext = mplayer_widetoutf8(mplayer->locations[i].path);
        #else
        music_location.utext = mplayer->locations[i].path;
        #endif
        mplayer_textmanager_sizetext(mplayer->font, &music_location);
        if(mplayer->settingmenu_contentcount == mplayer->settingmenu_scrollcontainer.content_renderpos) {
            music_location.text_canvas.y = mplayer->settingmenu_scrollcontainer.scroll_y;
        } else if(mplayer->settingmenu_contentcount < mplayer->settingmenu_scrollcontainer.content_renderpos) {
            mplayer_settingmenu_additem(mplayer, music_location.text_canvas);
            mplayer->settingmenu_contentcount++;
            #ifdef _WIN32
            free(music_location.utext); music_location.utext = NULL;
            #endif
            continue;
        }
        mplayer->settingmenu_contentcount++;
        mplayer_settingmenu_additem(mplayer, music_location.text_canvas);
        if(music_location.text_canvas.y < settingmenu_scrollcontainer->scroll_area.y +
            settingmenu_scrollcontainer->scroll_area.h) {
            mplayer_scrollcontainer_additem(settingmenu_scrollcontainer, music_location.text_canvas);
            mplayer_scrollcontainer_setnext_itemcanvas(settingmenu_scrollcontainer, music_location.text_canvas);
            iteration_count++;
            SDL_Texture* location_texture = mplayer_textmanager_renderunicode(mplayer, mplayer->font, &music_location);
            SDL_RenderCopy(mplayer->renderer, location_texture, NULL, &music_location.text_canvas);
            SDL_DestroyTexture(location_texture); location_texture = NULL;
        }
        music_location.text_canvas.y += music_location.text_canvas.h + SETTING_LINESPACING;
        #ifdef _WIN32
        free(music_location.utext); music_location.utext = NULL;
        #endif
    }
    printf("iteration_count: %zu, item_count: %zu\n", iteration_count,
        settingmenu_scrollcontainer->item_container.item_count);
}

void mplayer_settingmenu_render_personalization(mplayer_t* mplayer, SDL_Rect previous_canvas,
    mplayer_settingmenu_personalizationcategory_t* personalization_category) {
    personalization_category->text_info = setting_textinfo[2];
    personalization_category->text_info.text_canvas.y = previous_canvas.y + previous_canvas.h + SETTING_LINESPACING;
    mplayer_textmanager_sizetext(mplayer->font, &personalization_category->text_info);
    mplayer_settingmenu_additem(mplayer, personalization_category->text_info.text_canvas);
    mplayer->settingmenu_contentcount++;
    if(personalization_category->text_info.text_canvas.y < mplayer->settingmenu_scrollcontainer.scroll_area.y +
        mplayer->settingmenu_scrollcontainer.scroll_area.h) {
        mplayer_scrollcontainer_additem(&mplayer->settingmenu_scrollcontainer, personalization_category->text_info.text_canvas);
        mplayer_scrollcontainer_setnext_itemcanvas(&mplayer->settingmenu_scrollcontainer, personalization_category->text_info.text_canvas);
        SDL_Texture* personalization_text_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font,
        &personalization_category->text_info);
        SDL_RenderCopy(mplayer->renderer, personalization_text_texture, NULL,
            &personalization_category->text_info.text_canvas);
        SDL_DestroyTexture(personalization_text_texture); personalization_text_texture = NULL;
    }
}

void mplayer_settingmenu_render_about(mplayer_t* mplayer,
    mplayer_settingmenu_personalizationcategory_t personalization_category) {
    setting_textinfo[3].text_canvas.y = personalization_category.text_info.text_canvas.y +
        personalization_category.text_info.text_canvas.h + SETTING_LINESPACING;
    mplayer_settingmenu_aboutcategory_t about = {
        .text_info = setting_textinfo[3]
    };
    mplayer_textmanager_sizetext(mplayer->font, &about.text_info);
    mplayer_settingmenu_additem(mplayer, about.text_info.text_canvas);
    mplayer->settingmenu_contentcount++;
    if(about.text_info.text_canvas.y < mplayer->settingmenu_scrollcontainer.scroll_area.y +
        mplayer->settingmenu_scrollcontainer.scroll_area.h) {
        mplayer_scrollcontainer_additem(&mplayer->settingmenu_scrollcontainer, about.text_info.text_canvas);
        mplayer_scrollcontainer_setnext_itemcanvas(&mplayer->settingmenu_scrollcontainer, about.text_info.text_canvas);
        SDL_Texture* about_text_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font, &about.text_info);
        SDL_RenderCopy(mplayer->renderer, about_text_texture, NULL, &about.text_info.text_canvas);
        SDL_DestroyTexture(about_text_texture); about_text_texture = NULL;
    }
}