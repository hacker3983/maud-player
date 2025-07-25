#include "maud_settingsmenu.h"

void maud_settingmenu(maud_t* maud) {
    maud_itemcontainer_t* settingmenu_itemcontainer = &maud->settingmenu_itemcontainer;
    maud_scrollcontainer_t* settingmenu_scrollcontainer = &maud->settingmenu_scrollcontainer;
    maud_settingmenu_navbar_t navbar = {0};
    maud_settingmenu_librarycategory_t maud_librarycategory = {0};
    maud_settingmenu_personalizationcategory_t personalization_category = {0};
    while(SDL_PollEvent(&maud->e)) {
        if(maud->e.type == SDL_QUIT) {
            maud->quit = true; break;
        } else if(maud->e.type == SDL_MOUSEMOTION) {
            maud->mouse_x = maud->e.motion.x, maud->mouse_y = maud->e.motion.y;
        } else if(maud->e.type == SDL_MOUSEBUTTONDOWN) {
            maud->mouse_x = maud->e.button.x, maud->mouse_y = maud->e.button.y;
            maud->mouse_clicked = true;
        } else if(maud->e.type == SDL_MOUSEWHEEL) {
            maud_scrolltype_getmousewheel_scrolltype(maud->e, &maud->scroll_type);
            maud->scroll = true;
        }
    }
    maud_set_window_title(maud, SETTING_TITLE);
    maud_set_window_color(maud->renderer, setting_wincolor);
    SDL_GetWindowSize(maud->window, &maud->win_width, &maud->win_height);
    // Create the navigation bar and handle its components
    maud_settingmenu_create_navigationbar(maud, &navbar);
    if(maud_settingmenu_handle_backbtn(maud, &navbar)) {
        maud_itemcontainer_destroy(&maud->settingmenu_itemcontainer);
        maud_scrollcontainer_destroy(&maud->settingmenu_scrollcontainer);
        return;
    }
    SDL_Rect scroll_area = {
        .x = 0, .y = navbar.canvas.y + navbar.canvas.h,
        .w = maud->win_width, .h = maud->win_height - (navbar.canvas.y + navbar.canvas.h)
    };
    maud_scrollcontainer_setscroll_area(settingmenu_scrollcontainer, scroll_area);
    maud_settingmenu_render_musiclibrary(maud, navbar, &maud_librarycategory);
    maud_settingmenu_render_personalization(maud, settingmenu_itemcontainer->items[settingmenu_itemcontainer->item_index-1],
        &personalization_category);
    maud_settingmenu_render_about(maud, personalization_category);

    maud_scrollcontainer_setprops(&maud->settingmenu_scrollcontainer, scroll_area, 20, maud->settingmenu_contentcount);
    // Render the navigation bar
    if(maud->scroll) {
        printf("Scrolling\n");
        maud_scrollcontainer_performscroll_overscrollarea(maud, settingmenu_scrollcontainer);
        printf("settingmenu_itemcount: %zu, 2: %zu, scroll: %zu\n", settingmenu_itemcontainer->item_count, maud->settingmenu_contentcount,
            settingmenu_scrollcontainer->item_container.item_count);
        maud->scroll = false;
    }
    maud_settingmenu_handleupdate(maud);
    maud_itemcontainer_resetitem_index(settingmenu_itemcontainer);
    maud_scrollcontainer_resetitem_index(settingmenu_scrollcontainer);
    maud->settingmenu_contentcount = 0;
    maud_settingmenu_render_navigationbar(maud, &navbar);
    maud->mouse_clicked = false;
    maud_setcursor(maud, MAUD_CURSOR_DEFAULT);
    SDL_RenderPresent(maud->renderer);
}

void maud_settingmenu_handleupdate(maud_t* maud) {
    if(maud->settingmenu_scrollcontainer.item_container.items) {
        //maud_itemcontainer_set_hasallitems_status(&maud->settingmenu_scrollcontainer.item_container, true);
        maud_scrollcontainer_init(&maud->settingmenu_scrollcontainer);
    }
    if(maud->settingmenu_itemcontainer.item_count != maud->settingmenu_contentcount) {
        maud->settingmenu_itemcontainer.has_allitems = false;
    } else {
        maud_itemcontainer_set_hasallitems_status(&maud->settingmenu_itemcontainer, true);
    }
}

void maud_settingmenu_additem(maud_t* maud, SDL_Rect canvas) {
    maud_itemcontainer_t *settingmenu_itemcontainer = &maud->settingmenu_itemcontainer;
    if(settingmenu_itemcontainer->has_allitems) {
        //printf("has_allitems: true\n");
        maud_itemcontainer_setnextitem_canvas(settingmenu_itemcontainer, canvas);
        return;
    }
    if((!settingmenu_itemcontainer->items) ||
        settingmenu_itemcontainer->item_count < maud->settingmenu_contentcount) {
        maud_itemcontainer_additems(settingmenu_itemcontainer, 1);
    }
    maud_itemcontainer_setnextitem_canvas(settingmenu_itemcontainer, canvas);
}

void maud_settingmenu_create_navigationbar(maud_t* maud, maud_settingmenu_navbar_t* navbar_ref) {
    maud_textmanager_sizetext(maud->font, &setting_textinfo[0]);
    int max_navbarheight = (setting_btns[0].btn_canvas.h > setting_textinfo[0].text_canvas.h) ?
            setting_btns[0].btn_canvas.h : setting_textinfo[0].text_canvas.h;

    navbar_ref->color = (SDL_Color){0x39, 0x2F, 0x5A, 0xFF}/*{0x5E, 0xB1, 0xBF, 0xFF}{0x04, 0x2A, 0x2B, 0xFF}{0x00, 0xA6, 0x76, 0xff}*/;
    navbar_ref->canvas.x = 0, navbar_ref->canvas.y = 0;
    navbar_ref->canvas.w = maud->win_width, navbar_ref->canvas.h = max_navbarheight;

    // Center the settings button vertically on the navigation bar
    setting_btns[0].btn_canvas.y = navbar_ref->canvas.y + (navbar_ref->canvas.h - setting_btns[0].btn_canvas.h)/2;

    // Set "Settings" text position to come after the back button icons and center it vertically
    setting_textinfo[0].text_canvas.x = setting_btns[0].btn_canvas.x + setting_btns[0].btn_canvas.w + 10;
    setting_textinfo[0].text_canvas.y = navbar_ref->canvas.y + (navbar_ref->canvas.h -
        setting_textinfo[0].text_canvas.h)/2;

    navbar_ref->backbtn_canvas = setting_btns[0].btn_canvas;
    navbar_ref->backbtn_textinfo = setting_textinfo[0];

    maud_tooltip_t backbtn_tooltip = {
        .font = maud->music_font,
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
        .wrap_length = maud->win_width,
        .wrap_spacing = 5
    };
    navbar_ref->backbtn_tooltip = backbtn_tooltip;
}

bool maud_settingmenu_handle_backbtn(maud_t* maud, maud_settingmenu_navbar_t* navbar_ref) {
    if(maud_rect_hover(maud, navbar_ref->backbtn_canvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            maud->menu_opt = MAUD_DEFAULT_MENU;
            maud_menumanager_setup_menu(maud);
            maud->mouse_clicked = false;
            return true;
        }
    }
    return false;
}

void maud_settingmenu_render_navigationbar(maud_t* maud, maud_settingmenu_navbar_t* navbar_ref) {
    SDL_Texture* backbtn_texture = maud->menu->textures[MAUD_BUTTON_TEXTURE][setting_btns[0].texture_idx];
    SDL_Texture* backbtn_text_texture = maud_textmanager_rendertext(maud, maud->font, &setting_textinfo[0]);

    // Render the background for the navigation bar
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(navbar_ref->color));
    SDL_RenderDrawRect(maud->renderer, &navbar_ref->canvas);
    SDL_RenderFillRect(maud->renderer, &navbar_ref->canvas);

    // Render the contents of the navigation bar onto it's background
    SDL_RenderCopy(maud->renderer, backbtn_texture, NULL, &navbar_ref->backbtn_canvas);
    SDL_RenderCopy(maud->renderer, backbtn_text_texture, NULL, &navbar_ref->backbtn_textinfo.text_canvas);
    maud_tooltip_renderhover(maud, &navbar_ref->backbtn_tooltip);
}

void maud_settingmenu_render_musiclibrary(maud_t* maud, maud_settingmenu_navbar_t navbar,
    maud_settingmenu_librarycategory_t* library_category) {    
    library_category->text_info = setting_textinfo[1];
    maud_textmanager_sizetext(maud->font, &library_category->text_info);
    library_category->text_info.text_canvas.y = maud->settingmenu_scrollcontainer.scroll_y;
    maud->settingmenu_contentcount++;
    if(maud->settingmenu_itemcontainer.item_index == maud->settingmenu_scrollcontainer.content_renderpos) {
        if(library_category->text_info.text_canvas.y <
            maud->settingmenu_scrollcontainer.scroll_area.y + maud->settingmenu_scrollcontainer.scroll_area.h) {
                maud_scrollcontainer_additem(&maud->settingmenu_scrollcontainer, library_category->text_info.text_canvas);
                //printf("Increasing item count to %zu\n", maud->settingmenu_scrollcontainer.item_container.item_count);
                maud_scrollcontainer_setnext_itemcanvas(&maud->settingmenu_scrollcontainer, library_category->text_info.text_canvas);
                //printf("Added item to scroll container and item_count is %zu\n", maud->settingmenu_scrollcontainer.item_container.item_count);
        }
        // Render music library text
        SDL_Texture* library_text_texture = maud_textmanager_rendertext(maud, maud->font,
            &library_category->text_info);
        SDL_RenderCopy(maud->renderer, library_text_texture, NULL, &library_category->text_info.text_canvas);
        SDL_DestroyTexture(library_text_texture);
    }
    maud_settingmenu_additem(maud, library_category->text_info.text_canvas);

    // Render the music locations that contain the musics
    maud_settingmenu_render_musiclocations(maud, library_category->text_info.text_canvas);
}

void maud_settingmenu_render_musiclocations(maud_t* maud, SDL_Rect previous_canvas) {
    maud_scrollcontainer_t* settingmenu_scrollcontainer = &maud->settingmenu_scrollcontainer;
    maud_itemcontainer_t* settingmenu_itemcontainer = &maud->settingmenu_itemcontainer;
    text_info_t maud_location = {
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
    for(size_t i=0;i<maud->location_count;i++) {
        if(!maud->locations[i].path) {
            return;
        }
        //printf("maud->locations[%zu].path = %ls\n", i, maud->locations[i].path);
        #ifdef _WIN32
        maud_location.utext = maud_widetoutf8(maud->locations[i].path);
        #else
        maud_location.utext = maud->locations[i].path;
        #endif
        maud_textmanager_sizetext(maud->font, &maud_location);
        if(maud->settingmenu_contentcount == maud->settingmenu_scrollcontainer.content_renderpos) {
            maud_location.text_canvas.y = maud->settingmenu_scrollcontainer.scroll_y;
        } else if(maud->settingmenu_contentcount < maud->settingmenu_scrollcontainer.content_renderpos) {
            maud_settingmenu_additem(maud, maud_location.text_canvas);
            maud->settingmenu_contentcount++;
            #ifdef _WIN32
            free(maud_location.utext); maud_location.utext = NULL;
            #endif
            continue;
        }
        maud->settingmenu_contentcount++;
        maud_settingmenu_additem(maud, maud_location.text_canvas);
        if(maud_location.text_canvas.y < settingmenu_scrollcontainer->scroll_area.y +
            settingmenu_scrollcontainer->scroll_area.h) {
            maud_scrollcontainer_additem(settingmenu_scrollcontainer, maud_location.text_canvas);
            maud_scrollcontainer_setnext_itemcanvas(settingmenu_scrollcontainer, maud_location.text_canvas);
            iteration_count++;
            SDL_Texture* location_texture = maud_textmanager_renderunicode(maud, maud->font, &maud_location);
            SDL_RenderCopy(maud->renderer, location_texture, NULL, &maud_location.text_canvas);
            SDL_DestroyTexture(location_texture); location_texture = NULL;
        }
        maud_location.text_canvas.y += maud_location.text_canvas.h + SETTING_LINESPACING;
        #ifdef _WIN32
        free(maud_location.utext); maud_location.utext = NULL;
        #endif
    }
    printf("iteration_count: %zu, item_count: %zu\n", iteration_count,
        settingmenu_scrollcontainer->item_container.item_count);
}

void maud_settingmenu_render_personalization(maud_t* maud, SDL_Rect previous_canvas,
    maud_settingmenu_personalizationcategory_t* personalization_category) {
    personalization_category->text_info = setting_textinfo[2];
    personalization_category->text_info.text_canvas.y = previous_canvas.y + previous_canvas.h + SETTING_LINESPACING;
    maud_textmanager_sizetext(maud->font, &personalization_category->text_info);
    maud_settingmenu_additem(maud, personalization_category->text_info.text_canvas);
    maud->settingmenu_contentcount++;
    if(personalization_category->text_info.text_canvas.y < maud->settingmenu_scrollcontainer.scroll_area.y +
        maud->settingmenu_scrollcontainer.scroll_area.h) {
        maud_scrollcontainer_additem(&maud->settingmenu_scrollcontainer, personalization_category->text_info.text_canvas);
        maud_scrollcontainer_setnext_itemcanvas(&maud->settingmenu_scrollcontainer, personalization_category->text_info.text_canvas);
        SDL_Texture* personalization_text_texture = maud_textmanager_rendertext(maud, maud->font,
        &personalization_category->text_info);
        SDL_RenderCopy(maud->renderer, personalization_text_texture, NULL,
            &personalization_category->text_info.text_canvas);
        SDL_DestroyTexture(personalization_text_texture); personalization_text_texture = NULL;
    }
}

void maud_settingmenu_render_about(maud_t* maud,
    maud_settingmenu_personalizationcategory_t personalization_category) {
    setting_textinfo[3].text_canvas.y = personalization_category.text_info.text_canvas.y +
        personalization_category.text_info.text_canvas.h + SETTING_LINESPACING;
    maud_settingmenu_aboutcategory_t about = {
        .text_info = setting_textinfo[3]
    };
    maud_textmanager_sizetext(maud->font, &about.text_info);
    maud_settingmenu_additem(maud, about.text_info.text_canvas);
    maud->settingmenu_contentcount++;
    if(about.text_info.text_canvas.y < maud->settingmenu_scrollcontainer.scroll_area.y +
        maud->settingmenu_scrollcontainer.scroll_area.h) {
        maud_scrollcontainer_additem(&maud->settingmenu_scrollcontainer, about.text_info.text_canvas);
        maud_scrollcontainer_setnext_itemcanvas(&maud->settingmenu_scrollcontainer, about.text_info.text_canvas);
        SDL_Texture* about_text_texture = maud_textmanager_rendertext(maud, maud->font, &about.text_info);
        SDL_RenderCopy(maud->renderer, about_text_texture, NULL, &about.text_info.text_canvas);
        SDL_DestroyTexture(about_text_texture); about_text_texture = NULL;
    }
}