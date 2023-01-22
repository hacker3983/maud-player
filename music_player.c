#include "music_player.h"

SDL_Rect *playbtn_canvas = NULL, *prevbtn_canvas = NULL, *skipbtn_canvas = NULL;
void mplayer_init() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);
}

SDL_Window* mplayer_createwindow(const char* title, int width, int height) {
    SDL_Window* window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    width, height, SDL_WINDOW_RESIZABLE);
    return window;
}

SDL_Renderer* mplayer_createrenderer(SDL_Window* window) {
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    return renderer;
}

TTF_Font* mplayer_openfont(const char* file, int size) {
    TTF_Font* font = TTF_OpenFont(file, size);
    return font;
}

void mplayer_createall(mplayer_t* mplayer) {
    // create music player's graphical utilities
    mplayer->menu_opt = MPLAYER_DEFAULT_MENU;
    mplayer->window = mplayer_createwindow(WINDOW_TITLE, WIDTH, HEIGHT);
    mplayer->renderer = mplayer_createrenderer(mplayer->window);
    mplayer->font = mplayer_openfont(FONT_FILE, FONT_SIZE);
    mplayer->quit = 0;
    for(size_t i=0;i<MENU_COUNT;i++) {
        for(size_t j=0;j<TEXTURE_TYPECOUNT;j++) {
            mplayer->menus[i].obj_textures[j] = NULL;
            mplayer->menus[i].obj_canvases[j] = NULL;
        }
    }
    mplayer->menu = &mplayer->menus[mplayer->menu_opt];
    mplayer->cursors[MPLAYER_CURSOR_DEFAULT] = SDL_GetDefaultCursor();
    mplayer->cursors[MPLAYER_CURSOR_POINTER] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

    // create music information
    mplayer_getmusicpath_info(mplayer);
    if(mplayer->musinfo.locations == NULL) {
        #ifdef _WIN32
        char* username = getenv("USERNAME"), *location = NULL, root_path[4] = {0};
        location = calloc(16+strlen(username), sizeof(char));
        mplayer_getroot_path(root_path);
        strcpy(location, root_path);
        strcat(location, "Users\\");
        strcat(location, username);
        strcat(location, "\\Music");
        mplayer_addmusic_location(mplayer, location);
        free(location);
        #endif
    }
}

void mplayer_set_window_color(SDL_Renderer* renderer, SDL_Color bg_color) {
    SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    SDL_RenderClear(renderer);
}

void mplayer_set_window_title(mplayer_t* mplayer, const char* title) {
    SDL_SetWindowTitle(mplayer->window, title);
}

void mplayer_setcursor(mplayer_t* mplayer, int cursor_type) {
    switch(cursor_type) {
        case MPLAYER_CURSOR_DEFAULT:
            SDL_SetCursor(mplayer->cursors[cursor_type]);
        case MPLAYER_CURSOR_POINTER:
            SDL_SetCursor(mplayer->cursors[cursor_type]);
    }
}

bool mplayer_tab_hover(mplayer_t* mplayer, tabinfo_t tab) {
    int x = tab.text_canvas.x, y = tab.text_canvas.y,
        w = tab.text_canvas.w, h = tab.text_canvas.h;
    if((mplayer->e.motion.x <= x + w && mplayer->e.motion.x >= x) &&
        (mplayer->e.motion.y <= y + h && mplayer->e.motion.y >= y)) {
        return true;
    }
    return false;
}

bool mplayer_ibutton_hover(mplayer_t* mplayer, ibtn_t button) {
    int x = button.btn_canvas.x, y = button.btn_canvas.y,
        w = button.btn_canvas.w, h = button.btn_canvas.h;
    if((mplayer->e.motion.x <= x + h && mplayer->e.motion.x >= x) &&
        (mplayer->e.motion.y <= y + h && mplayer->e.motion.y >= y)) {
            return true;
    }
    return false;
}

bool mplayer_tbutton_hover(mplayer_t* mplayer, tbtn_t button) {
    int x = button.btn_canvas.x, y = button.btn_canvas.y,
        w = button.btn_canvas.w, h = button.btn_canvas.h;
    if((mplayer->e.motion.x <= x + h && mplayer->e.motion.x >= x) &&
        (mplayer->e.motion.y <= y + h && mplayer->e.motion.y >= y)) {
            return true;
    }
    return false;
}

bool mplayer_tabs_hover(mplayer_t* mplayer, tabinfo_t* tabs, int* tab_id, size_t tab_count) {
    for(size_t i=0;i<tab_count;i++) {
        if(mplayer_tab_hover(mplayer, tabs[i])) {
            if(tab_id != NULL) {
                *tab_id = tabs[i].id;
            }
            tabs[i].hover = true; return true;
        }
        tabs[i].hover = false;
    }
    return false;
}

bool mplayer_ibuttons_hover(mplayer_t* mplayer, ibtn_t* buttons, int* btn_id, size_t button_count) {
    for(size_t i=0;i<button_count;i++) {
        if(mplayer_ibutton_hover(mplayer, buttons[i])) {
            if(btn_id != NULL) {
                *btn_id = buttons[i].id;
            }
            buttons[i].hover = true; return true;
        }
    }
    return false;
}

bool mplayer_tbuttons_hover(mplayer_t* mplayer, tbtn_t* buttons, int* btn_id, size_t button_count) {
    for(size_t i=0;i<button_count;i++) {
        if(mplayer_tbutton_hover(mplayer, buttons[i])) {
            if(btn_id != NULL) {
                *btn_id = buttons[i].id;
            }
            buttons[i].hover = true; return true;
        }
    }
}

SDL_Texture* mplayer_rendertext(mplayer_t* mplayer, text_info_t* text_info) {
    SDL_Rect text_canvas = text_info->text_canvas;
    TTF_SetFontSize(mplayer->font, text_info->font_size);
    TTF_SizeText(mplayer->font, text_info->text, &text_canvas.w, &text_canvas.h);
    SDL_Surface* surface = TTF_RenderText_Blended(mplayer->font, text_info->text, text_info->text_color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(mplayer->renderer, surface);
    SDL_FreeSurface(surface);
    text_info->text_canvas = text_canvas;
    return texture;
}

SDL_Texture* mplayer_rendertab(mplayer_t* mplayer, tabinfo_t* tab_info) {
    SDL_Rect text_canvas = tab_info->text_canvas;
    TTF_SetFontSize(mplayer->font, tab_info->font_size);
    TTF_SizeText(mplayer->font, tab_info->text, &text_canvas.w, &text_canvas.h);
    SDL_Surface* surface = TTF_RenderText_Blended(mplayer->font, tab_info->text, tab_info->text_color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(mplayer->renderer, surface);
    SDL_FreeSurface(surface);
    tab_info->text_canvas = text_canvas;
    return texture;
}

void mplayer_renderactive_tab(mplayer_t* mplayer, tabinfo_t* tab_info) {
    if(!tab_info->active) return;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(tab_info->underline_color));
    for(int i=0;i<UNDERLINE_THICKNESS;i++) {
        SDL_RenderDrawLine(mplayer->renderer, tab_info->text_canvas.x,
        tab_info->text_canvas.y+i + tab_info->text_canvas.h, tab_info->text_canvas.x+tab_info->text_canvas.w,
        tab_info->text_canvas.y+i + tab_info->text_canvas.h);
    }
}

void mplayer_centertext(mplayer_t* mplayer, text_info_t* text_info) {
    SDL_Rect text_canvas = text_info->text_canvas;
    text_canvas.x = (WIDTH - text_canvas.w) / 2, text_canvas.y = (WIDTH - text_canvas.h) / 2;
    text_info->text_canvas = text_canvas;
}

void mplayer_centerx(mplayer_t* mplayer, text_info_t* text_info) {
    SDL_Rect text_canvas = text_info->text_canvas;
    text_canvas.x = (WIDTH- text_canvas.w) /2;
    text_info->text_canvas = text_canvas;
}

void mplayer_centery(mplayer_t* mplayer, text_info_t* text_info) {
    SDL_Rect text_canvas = text_info->text_canvas;
    text_canvas.y = (WIDTH - text_canvas.h)/2;
    text_info->text_canvas = text_canvas;
}

void mplayer_destroytextures(SDL_Texture** textures, size_t n) {
    for(size_t i=0;i<n;i++) {
        if(textures[i] != NULL) {
            SDL_DestroyTexture(textures[i]);
            textures[i] = NULL;
        }
    }
}

void mplayer_destroyall(mplayer_t* mplayer) {
    // destroys music player graphrical utilities
    SDL_DestroyRenderer(mplayer->renderer);
    SDL_DestroyWindow(mplayer->window);
    TTF_CloseFont(mplayer->font);

    // Free resources used by program
    mplayer_freemusic_info(mplayer);

    mplayer_destroytextures(mplayer->menus[MPLAYER_DEFAULT_MENU].obj_textures[MPLAYER_BUTTON_TEXTURE], MTOTALBTN_COUNT);
    for(size_t i=0;i<MENU_COUNT;i++) {
        free(mplayer->menus[i].obj_canvases[MPLAYER_TEXT_TEXTURE]);
        free(mplayer->menus[i].obj_canvases[MPLAYER_BUTTON_TEXTURE]);
        free(mplayer->menus[i].obj_canvases[MPLAYER_TAB_TEXTURE]);

        mplayer->menus[i].obj_textures[MPLAYER_TEXT_TEXTURE] = NULL;
        mplayer->menus[i].obj_textures[MPLAYER_BUTTON_TEXTURE] = NULL;
        mplayer->menus[i].obj_textures[MPLAYER_TAB_TEXTURE] = NULL;

        mplayer->menus[i].obj_canvases[MPLAYER_TEXT_TEXTURE] = NULL;
        mplayer->menus[i].obj_canvases[MPLAYER_BUTTON_TEXTURE] = NULL;
        mplayer->menus[i].obj_canvases[MPLAYER_TAB_TEXTURE] = NULL;
    }
    SDL_FreeCursor(mplayer->cursors[MPLAYER_CURSOR_POINTER]);

    // uninitialize libraries
    TTF_Quit();
    SDL_Quit();
}

void mplayer_createmusicbar(mplayer_t* mplayer/*, SDL_Texture* musicbtn_textures[]*/) {
    playbtn_canvas = &music_btns[MUSIC_PLAYBTN].btn_canvas,
        skipbtn_canvas = &music_btns[MUSIC_SKIPBTN].btn_canvas,
        prevbtn_canvas = &music_btns[MUSIC_PREVBTN].btn_canvas;
    // create music bar
    music_status.y = HEIGHT - music_status.h, music_status.w = WIDTH;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(music_statusbar_color));
    SDL_RenderFillRect(mplayer->renderer, &music_status);
    SDL_RenderDrawRect(mplayer->renderer, &music_status);

    // place the play, previous and skip button in the middle of the music status bar
    playbtn_canvas->x = (WIDTH - playbtn_canvas->w)/2, playbtn_canvas->y = HEIGHT - (playbtn_canvas->h +
        (playbtn_canvas->h/2));
    prevbtn_canvas->x = (WIDTH - prevbtn_canvas->w)/4, prevbtn_canvas->y = HEIGHT - (prevbtn_canvas->h +
       (prevbtn_canvas->h/2));
    skipbtn_canvas->x = (WIDTH - (skipbtn_canvas->w))/2 + (WIDTH - skipbtn_canvas->w)/4, skipbtn_canvas->y = HEIGHT -
        (skipbtn_canvas->h + (skipbtn_canvas->h/2));

    // render all buttons on music status bar
    for(int i=0;i<MUSICBTN_COUNT;i++) {
        mplayer->menu->obj_canvases[MPLAYER_BUTTON_TEXTURE][i] = music_btns[i].btn_canvas;
        SDL_RenderCopy(mplayer->renderer, mplayer->menu->obj_textures[MPLAYER_BUTTON_TEXTURE][i],
            NULL, &music_btns[i].btn_canvas);
    }
}

void mplayer_createsongs_box(mplayer_t* mplayer) {
    songs_box.y = tab_info[0].text_canvas.y + tab_info[0].text_canvas.h + (UNDERLINE_THICKNESS + 5);
    songs_box.w = WIDTH, songs_box.h = HEIGHT - music_status.h - songs_box.y;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(songs_boxcolor));
    SDL_RenderDrawRect(mplayer->renderer, &songs_box);
}

void mplayer_setup_menu(mplayer_t* mplayer) {
    mplayer_menu_t* menu = &mplayer->menus[mplayer->menu_opt];
    if(mplayer->menu_opt == MPLAYER_DEFAULT_MENU && menu->obj_textures[MPLAYER_TEXT_TEXTURE] == NULL) {
        menu->obj_textures[MPLAYER_TEXT_TEXTURE] = calloc(text_info_size-1, sizeof(SDL_Texture*));
        menu->obj_textures[MPLAYER_TAB_TEXTURE] = calloc(tab_info_size, sizeof(SDL_Texture*));
        menu->obj_textures[MPLAYER_BUTTON_TEXTURE] = calloc(MTOTALBTN_COUNT, sizeof(SDL_Texture*));

        menu->obj_canvases[MPLAYER_TEXT_TEXTURE] = calloc(text_info_size-1, sizeof(SDL_Rect));
        menu->obj_canvases[MPLAYER_TAB_TEXTURE] = calloc(tab_info_size, sizeof(SDL_Rect));
        menu->obj_canvases[MPLAYER_BUTTON_TEXTURE] = calloc(MTOTALBTN_COUNT, sizeof(SDL_Rect));

        // Load Button Textures and Canvas's
        for(int i=0;i<MUSICBTN_COUNT;i++) {
            menu->obj_textures[MPLAYER_BUTTON_TEXTURE][i] = IMG_LoadTexture(mplayer->renderer, music_btns[i].imgbtn_path);
            menu->obj_canvases[MPLAYER_BUTTON_TEXTURE][i] = music_btns[i].btn_canvas;
        }
        menu->obj_textures[MPLAYER_BUTTON_TEXTURE][MTOTALBTN_COUNT - 1] = IMG_LoadTexture(mplayer->renderer,
            setting_iconbtn.imgbtn_path);
        menu->obj_canvases[MPLAYER_BUTTON_TEXTURE][MTOTALBTN_COUNT - 1] = setting_iconbtn.btn_canvas;
    } else if(mplayer->menu_opt == MPLAYER_SETTINGS_MENU && menu->obj_textures[MPLAYER_TEXT_TEXTURE] == NULL) {
        menu->obj_textures[MPLAYER_TEXT_TEXTURE] = calloc(setting_textinfo_size, sizeof(SDL_Texture*));
        menu->obj_canvases[MPLAYER_TEXT_TEXTURE] = calloc(setting_textinfo_size, sizeof(SDL_Rect));

        menu->obj_textures[MPLAYER_BUTTON_TEXTURE] = calloc(SETTINGSBTN_COUNT, sizeof(SDL_Texture*));
        menu->obj_canvases[MPLAYER_BUTTON_TEXTURE] = calloc(SETTINGSBTN_COUNT, sizeof(SDL_Rect));

        for(size_t i=0;i<SETTINGSBTN_COUNT;i++) {
            menu->obj_textures[MPLAYER_BUTTON_TEXTURE][i] = IMG_LoadTexture(mplayer->renderer, setting_btns[i].imgbtn_path);
            menu->obj_canvases[MPLAYER_BUTTON_TEXTURE][i] = setting_btns[i].btn_canvas;
        }
    }
    mplayer->menu = menu;
}

void mplayer_run(mplayer_t* mplayer) {
    mplayer_setup_menu(mplayer);
    while(!mplayer->quit) {
        if(mplayer->menu_opt == MPLAYER_DEFAULT_MENU) {
            mplayer_defaultmenu(mplayer);
        } else if(mplayer->menu_opt == MPLAYER_SETTINGS_MENU) {
            mplayer_settingmenu(mplayer);
        }
    }
}

void mplayer_defaultmenu(mplayer_t* mplayer) {
    int tab_hoverid = 0;
    mplayer_set_window_title(mplayer, WINDOW_TITLE);
    while(SDL_PollEvent(&mplayer->e)) {
        if(mplayer->e.type == SDL_QUIT) {
                mplayer->quit = 1; break;
        } else if(mplayer->e.type == SDL_MOUSEMOTION) {
            if(mplayer_ibutton_hover(mplayer, setting_iconbtn)) {
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
                setting_iconbtn.hover = true;
            } else if(mplayer_tabs_hover(mplayer, tab_info, &tab_hoverid, tab_info_size) && TAB_INIT) {
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
            } else {
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_DEFAULT);
                setting_iconbtn.hover = false;
            }
        } else if(mplayer->e.type == SDL_MOUSEBUTTONUP) {
            if(mplayer_tabs_hover(mplayer, tab_info, &tab_hoverid, tab_info_size) && TAB_INIT) {
                tab_info[prev_tab].active = false;
                tab_info[tab_hoverid].underline_color = underline_color;
                tab_info[tab_hoverid].active = true;
                active_tab = tab_hoverid;
            } else if(mplayer_ibutton_hover(mplayer, setting_iconbtn)) {
                mplayer->menu_opt = MPLAYER_SETTINGS_MENU;
                mplayer_setup_menu(mplayer);
                return;
            }
        } else if(mplayer->e.type == SDL_MOUSEWHEEL) {
            if(mplayer->e.wheel.y < 0) {
                printf("Scroll Down event detected on y axis: %d\n", mplayer->e.wheel.mouseY);
            } else if(mplayer->e.wheel.y > 0) {
                printf("Scroll Up event detect on y axis: %d\n", mplayer->e.wheel.mouseY);
            }
        }
    }
    SDL_GetWindowSize(mplayer->window, &WIDTH, &HEIGHT);
    mplayer_set_window_color(mplayer->renderer, window_color);
    for(int i=0;i<text_info_size-2;i++) {
        mplayer->menu->obj_textures[MPLAYER_TEXT_TEXTURE][i] = mplayer_rendertext(mplayer, &text_info[i]);
        mplayer->menu->obj_canvases[MPLAYER_TEXT_TEXTURE][i] = text_info[i].text_canvas;
        SDL_RenderCopy(mplayer->renderer, mplayer->menu->obj_textures[MPLAYER_TEXT_TEXTURE][i], NULL,
            &text_info[i].text_canvas);
    }
    SDL_Rect tab_canvas = tab_info[0].text_canvas, text_canvas = text_info[0].text_canvas;
    if(tab_canvas.x != text_canvas.x + text_canvas.w + TAB_SPACING) {
        tab_info[0].text_canvas.x = text_canvas.x + text_canvas.w + TAB_SPACING;
    }
    if(!TAB_INIT) {
        TAB_INIT = 1;
    }
    for(int i=1;i<tab_info_size+1;i++) {
        mplayer->menu->obj_textures[MPLAYER_TAB_TEXTURE][i-1] = mplayer_rendertab(mplayer, &tab_info[i-1]);
        if(i+1 > 1) {
            int tab_x = tab_info[i-1].text_canvas.x + tab_info[i-1].text_canvas.w + TAB_SPACING;
            tab_info[i].text_canvas.x = tab_x;
        }
        if(tab_info[i-1].active) {
            mplayer_renderactive_tab(mplayer, &tab_info[i-1]);
        }
        SDL_RenderCopy(mplayer->renderer, mplayer->menu->obj_textures[MPLAYER_TAB_TEXTURE][i-1], NULL,
            &tab_info[i-1].text_canvas);
        mplayer->menu->obj_canvases[MPLAYER_TAB_TEXTURE][i-1] = tab_info[i-1].text_canvas;
    }
    if(active_tab == SONGS_TAB) {
        SDL_Texture* text_texture = mplayer_rendertext(mplayer, &text_info[1]);
        mplayer->menu->obj_textures[MPLAYER_TEXT_TEXTURE][1] = text_texture;
        text_info[1].text_canvas.x = (WIDTH - text_info[1].text_canvas.w)/2,
        text_info[1].text_canvas.y = (HEIGHT - text_info[1].text_canvas.h)/2;
        mplayer->menu->obj_canvases[MPLAYER_TEXT_TEXTURE][1] = text_info[1].text_canvas;
        SDL_RenderCopy(mplayer->renderer, mplayer->menu->obj_textures[MPLAYER_TEXT_TEXTURE][1], NULL,
            &text_info[1].text_canvas);
    } else if(active_tab == ALBUMS_TAB) {
        SDL_Texture* text_texture = mplayer_rendertext(mplayer, &text_info[2]);
        mplayer->menu->obj_textures[MPLAYER_TEXT_TEXTURE][1] = text_texture;
        text_info[2].text_canvas.x = (WIDTH - text_info[2].text_canvas.w)/2,
        text_info[2].text_canvas.y = (HEIGHT - text_info[2].text_canvas.h)/2;
        mplayer->menu->obj_canvases[MPLAYER_TEXT_TEXTURE][1] = text_info[2].text_canvas;
        SDL_RenderCopy(mplayer->renderer, mplayer->menu->obj_textures[MPLAYER_TEXT_TEXTURE][1], NULL,
            &text_info[2].text_canvas);
    }
    prev_tab = active_tab;

    // Create settings button on screen
    setting_iconbtn.btn_canvas.x = WIDTH - setting_iconbtn.btn_canvas.w - 2;
    mplayer->menu->obj_canvases[MPLAYER_BUTTON_TEXTURE][MTOTALBTN_COUNT-1] = setting_iconbtn.btn_canvas;
    SDL_RenderCopy(mplayer->renderer, mplayer->menu->obj_textures[MPLAYER_BUTTON_TEXTURE][MTOTALBTN_COUNT-1], NULL,
        &setting_iconbtn.btn_canvas);

    // create the scroll bar
    scrollbar.y = (HEIGHT - scrollbar.h)/2, scrollbar.x = WIDTH - (scrollbar.w+10);
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(white));
    SDL_RenderFillRect(mplayer->renderer, &scrollbar);
    SDL_RenderDrawRect(mplayer->renderer, &scrollbar);

    /* Create songs box*/
    mplayer_createsongs_box(mplayer);
    /* Create music bar */
    mplayer_createmusicbar(mplayer);

    // Present all rendered objects on the screen
    SDL_RenderPresent(mplayer->renderer);
    mplayer_destroytextures(mplayer->menu->obj_textures[MPLAYER_TEXT_TEXTURE], text_info_size-1);
    mplayer_destroytextures(mplayer->menu->obj_textures[MPLAYER_TAB_TEXTURE], tab_info_size);
}

void mplayer_settingmenu(mplayer_t* mplayer) {
    SDL_Rect* canvas = NULL;
    int btn_id = 0;
    mplayer_set_window_title(mplayer, SETTING_TITLE);
    while(SDL_PollEvent(&mplayer->e)) {
        if(mplayer->e.type == SDL_QUIT) {
            mplayer->quit = 1;
            break;
        } else if(mplayer->e.type == SDL_MOUSEMOTION) {
            // Check if we hover the back button
            if(mplayer_ibuttons_hover(mplayer, setting_btns, &btn_id, SETTINGSBTN_COUNT)) {
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
            } else {
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_DEFAULT);
            }
        } else if(mplayer->e.type == SDL_MOUSEBUTTONUP) {
            if(mplayer_ibuttons_hover(mplayer, setting_btns, &btn_id, SETTINGSBTN_COUNT)) {
                mplayer->menu_opt = MPLAYER_DEFAULT_MENU;
                mplayer_setup_menu(mplayer);
            }
        }
    }
    mplayer_set_window_color(mplayer->renderer, setting_wincolor);

    // create settings button and display on the screen
    for(size_t i=0;i<setting_textinfo_size;i++) {
        mplayer->menu->obj_textures[MPLAYER_TEXT_TEXTURE][i] = mplayer_rendertext(mplayer, &setting_textinfo[i]);
    }

    // set the canvas y position of the text "Settings"
    canvas = &setting_textinfo[0].text_canvas;
    canvas->y = canvas->h + SETTING_LINESPACING;

    // underline the settings text
    for(int i=0;i<10;i++) {
        SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(setting_underlinecolor));
        SDL_RenderDrawLine(mplayer->renderer, canvas->x, canvas->y + canvas->h + i, canvas->x + canvas->w,
            canvas->y + canvas->h + i);
    }

    // set the canvas x position of the text "Go Back To Home" beside the back button 
    canvas = &setting_textinfo[1].text_canvas;
    canvas->x = setting_btns[0].btn_canvas.x + setting_btns[0].btn_canvas.w + 5;


    // render each texture on its particular canvas
    for(size_t i=0;i<setting_textinfo_size;i++) {
        mplayer->menu->obj_canvases[MPLAYER_TEXT_TEXTURE][i] = setting_textinfo[i].text_canvas;
        SDL_RenderCopy(mplayer->renderer, mplayer->menu->obj_textures[MPLAYER_TEXT_TEXTURE][i], NULL,
            &mplayer->menu->obj_canvases[MPLAYER_TEXT_TEXTURE][i]);
    }

    // render buttons on screen
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(back_btnbg_color));
    SDL_RenderDrawRect(mplayer->renderer, &setting_btns[0].btn_canvas);
    SDL_RenderFillRect(mplayer->renderer, &setting_btns[0].btn_canvas);
    for(size_t i=0;i<SETTINGSBTN_COUNT;i++) {
        SDL_RenderCopy(mplayer->renderer, mplayer->menu->obj_textures[MPLAYER_BUTTON_TEXTURE][i], NULL,
            &mplayer->menu->obj_canvases[MPLAYER_BUTTON_TEXTURE][i]);
    }
    SDL_RenderPresent(mplayer->renderer);
    mplayer_destroytextures(mplayer->menu->obj_textures[MPLAYER_TEXT_TEXTURE], setting_textinfo_size);
}