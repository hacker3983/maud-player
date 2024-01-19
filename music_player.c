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

void mplayer_createapp(mplayer_t* mplayer) {
    // create music player's graphical utilities
    mplayer->menu_opt = MPLAYER_DEFAULT_MENU;
    mplayer->window = mplayer_createwindow(WINDOW_TITLE, WIDTH, HEIGHT);
    mplayer->renderer = mplayer_createrenderer(mplayer->window);
    mplayer->font = mplayer_openfont(FONT_FILE, FONT_SIZE);
    mplayer->quit = 0;
    for(size_t i=0;i<MENU_COUNT;i++) {
        for(size_t j=0;j<TEXTURE_TYPECOUNT;j++) {
            mplayer->menus[i].textures[j] = NULL;
            mplayer->menus[i].texture_canvases[j] = NULL;
            mplayer->menus[i].texture_sizes[j] = 0;
        }
        mplayer->menus[i].texts = NULL;
        mplayer->menus[i].canvases = NULL;
        mplayer->menus[i].text_count = 0;
        mplayer->menus[i].canvas_count = 0;
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
        // TODO: Linux
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
    if(textures == NULL) {
        return;
    }
    for(size_t i=0;i<n;i++) {
        if(textures[i] != NULL) {
            SDL_DestroyTexture(textures[i]);
            textures[i] = NULL;
        }
    }
}

void mplayer_menu_freetext(mplayer_t* mplayer, int menu_option) {
    free(mplayer->menus[menu_option].texts);
    mplayer->menus[menu_option].texts = NULL;
    mplayer->menus[menu_option].text_count = 0;
}

void mplayer_destroyapp(mplayer_t* mplayer) {
    // destroys music player graphrical utilities
    SDL_DestroyRenderer(mplayer->renderer);
    SDL_DestroyWindow(mplayer->window);
    TTF_CloseFont(mplayer->font);

    // Free resources used by program
    mplayer_freemusic_info(mplayer);

    // free the text informations
    mplayer_menu_freetext(mplayer, MPLAYER_DEFAULT_MENU);
    mplayer_menu_freetext(mplayer, MPLAYER_SETTINGS_MENU);

    // free texture objects for default menu
    mplayer_destroytextures(mplayer->menus[MPLAYER_DEFAULT_MENU].textures[MPLAYER_BUTTON_TEXTURE],
    mplayer->menus[MPLAYER_DEFAULT_MENU].texture_sizes[MPLAYER_BUTTON_TEXTURE]);

    // free texture objects for setting menu
    mplayer_destroytextures(mplayer->menus[MPLAYER_SETTINGS_MENU].textures[MPLAYER_TEXT_TEXTURE],
        mplayer->menus[MPLAYER_SETTINGS_MENU].texture_sizes[MPLAYER_TEXT_TEXTURE]);
    mplayer_destroytextures(mplayer->menus[MPLAYER_SETTINGS_MENU].textures[MPLAYER_BUTTON_TEXTURE],
        mplayer->menus[MPLAYER_SETTINGS_MENU].texture_sizes[MPLAYER_BUTTON_TEXTURE]);

    for(size_t i=0;i<MENU_COUNT;i++) {
        free(mplayer->menus[i].texture_canvases[MPLAYER_TEXT_TEXTURE]);
        free(mplayer->menus[i].texture_canvases[MPLAYER_BUTTON_TEXTURE]);
        free(mplayer->menus[i].texture_canvases[MPLAYER_TAB_TEXTURE]);

        free(mplayer->menus[i].textures[MPLAYER_TEXT_TEXTURE]);
        free(mplayer->menus[i].textures[MPLAYER_BUTTON_TEXTURE]);
        free(mplayer->menus[i].textures[MPLAYER_TAB_TEXTURE]);

        mplayer->menus[i].textures[MPLAYER_TEXT_TEXTURE] = NULL;
        mplayer->menus[i].textures[MPLAYER_BUTTON_TEXTURE] = NULL;
        mplayer->menus[i].textures[MPLAYER_TAB_TEXTURE] = NULL;

        mplayer->menus[i].texture_canvases[MPLAYER_TEXT_TEXTURE] = NULL;
        mplayer->menus[i].texture_canvases[MPLAYER_BUTTON_TEXTURE] = NULL;
        mplayer->menus[i].texture_canvases[MPLAYER_TAB_TEXTURE] = NULL;
    }
    SDL_FreeCursor(mplayer->cursors[MPLAYER_CURSOR_POINTER]);

    // uninitialize libraries
    IMG_Quit();
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
        mplayer->menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][i] = music_btns[i].btn_canvas;
        SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][i],
            NULL, &music_btns[i].btn_canvas);
    }
}

void mplayer_createsongs_box(mplayer_t* mplayer) {
    songs_box.y = tab_info[0].text_canvas.y + tab_info[0].text_canvas.h + (UNDERLINE_THICKNESS + 5);
    songs_box.w = WIDTH, songs_box.h = HEIGHT - music_status.h - songs_box.y;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(songs_boxcolor));
    SDL_RenderDrawRect(mplayer->renderer, &songs_box);
}

void mplayer_menu_appendtext(mplayer_t* mplayer, text_info_t text) {
    if(!mplayer->menu->texts) {
        mplayer->menu->texts = calloc(1, sizeof(text_info_t));
        mplayer->menu->texts[0] = text;
        mplayer->menu->text_count++;
        return;
    }
    mplayer->menu->text_count++;
    mplayer->menu->texts = realloc(mplayer->menu->texts,
        (mplayer->menu->text_count) * sizeof(text_info_t));
    mplayer->menu->texts[mplayer->menu->text_count-1] = text;
}

SDL_Texture** mplayer_createtexture_list(size_t amount) {
    SDL_Texture** objtexture_list = calloc(amount, sizeof(SDL_Texture*));
    return objtexture_list;
}

void mplayer_realloctexture(SDL_Texture*** texture_objs, size_t* amount) {
    (*amount)++;
    (*texture_objs) = realloc(*texture_objs, (*amount) * sizeof(SDL_Texture*));
    (*texture_objs)[(*amount)-1] = NULL;
}

void mplayer_createmenu_texture(mplayer_t* mplayer, int texture_type, size_t amount) {
    mplayer->menu->textures[texture_type] = mplayer_createtexture_list(amount);
    mplayer->menu->texture_canvases[texture_type] = calloc(amount, sizeof(SDL_Rect));
    mplayer->menu->texture_sizes[texture_type] = amount;
}

void mplayer_addmenu_texture(mplayer_t* mplayer, int texture_type) {
    size_t *amount = &mplayer->menu->texture_sizes[texture_type];
    mplayer_realloctexture(&mplayer->menu->textures[texture_type],
       amount);
    mplayer->menu->texture_canvases[texture_type] = realloc(mplayer->menu->texture_canvases[texture_type],
        (*amount) * sizeof(SDL_Rect));
    memset(mplayer->menu->texture_canvases[texture_type], 0, (*amount) * sizeof(SDL_Rect));
}

void mplayer_menuplace_texture(mplayer_t* mplayer, int type, SDL_Texture* texture, SDL_Rect canvas) {
    size_t texture_size = mplayer->menu->texture_sizes[type];
    mplayer->menu->textures[type][texture_size-1] = texture;
    mplayer->menu->texture_canvases[type][texture_size-1] = canvas;
}

void mplayer_menuadd_canvas(mplayer_t* mplayer, SDL_Rect canvas) {
    size_t* canvas_count = &mplayer->menu->canvas_count;
    if(canvas_count == 0) {
        mplayer->menu->canvases = calloc(1, sizeof(SDL_Rect));
        mplayer->menu->canvases[(*canvas_count)++] = canvas;
        return;
    }
    mplayer->menu->canvases = realloc(mplayer->menu->canvases, ((*canvas_count)+1) * sizeof(SDL_Rect));
    mplayer->menu->canvases[*canvas_count] = canvas;
    (*canvas_count)++;
}

void mplayer_setup_menu(mplayer_t* mplayer) {
    mplayer_menu_t* menu = &mplayer->menus[mplayer->menu_opt];
    mplayer->menu = menu;
    if(mplayer->menu_opt == MPLAYER_DEFAULT_MENU && menu->textures[MPLAYER_TEXT_TEXTURE] == NULL) {
        mplayer_createmenu_texture(mplayer, MPLAYER_TEXT_TEXTURE, text_info_size-1);
        mplayer_createmenu_texture(mplayer, MPLAYER_TAB_TEXTURE, tab_info_size);
        mplayer_createmenu_texture(mplayer, MPLAYER_BUTTON_TEXTURE, MTOTALBTN_COUNT);
        // Load Button Textures and Canvas's
        for(int i=0;i<MUSICBTN_COUNT;i++) {
            menu->textures[MPLAYER_BUTTON_TEXTURE][i] = IMG_LoadTexture(mplayer->renderer, music_btns[i].imgbtn_path);
            menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][i] = music_btns[i].btn_canvas;
        }
        menu->textures[MPLAYER_BUTTON_TEXTURE][MTOTALBTN_COUNT - 1] = IMG_LoadTexture(mplayer->renderer,
            setting_iconbtn.imgbtn_path);
        menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][MTOTALBTN_COUNT - 1] = setting_iconbtn.btn_canvas;
    } else if(mplayer->menu_opt == MPLAYER_SETTINGS_MENU && menu->textures[MPLAYER_TEXT_TEXTURE] == NULL) {
        mplayer_createmenu_texture(mplayer, MPLAYER_TEXT_TEXTURE, setting_textinfo_size);
        mplayer_createmenu_texture(mplayer, MPLAYER_BUTTON_TEXTURE, SETTINGSBTN_COUNT);
        for(size_t i=0;i<SETTINGSBTN_COUNT;i++) {
            menu->textures[MPLAYER_BUTTON_TEXTURE][i] = IMG_LoadTexture(mplayer->renderer, setting_btns[i].imgbtn_path);
            menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][i] = setting_btns[i].btn_canvas;
        }
    }
}

void mplayer_run(mplayer_t* mplayer) {
    mplayer_setup_menu(mplayer);
    mplayer_loadmusics(mplayer);
    /*for(size_t i=0;i<mplayer->musinfo.file_count;i++) {
        printf("%s\n", mplayer->music_list[i].music_name);
    }*/
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
        }
    }
    SDL_GetWindowSize(mplayer->window, &WIDTH, &HEIGHT);
    mplayer_set_window_color(mplayer->renderer, window_color);
    mplayer->menu->textures[MPLAYER_TEXT_TEXTURE][0] = mplayer_rendertext(mplayer, &text_info[0]);
    mplayer->menu->texture_canvases[MPLAYER_TEXT_TEXTURE][0] = text_info[0].text_canvas;
    //printf("1\n");
    SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_TEXT_TEXTURE][0], NULL,
            &text_info[0].text_canvas);
    //printf("2\n");
    SDL_Rect tab_canvas = tab_info[0].text_canvas, text_canvas = text_info[0].text_canvas;
    if(tab_canvas.x != text_canvas.x + text_canvas.w + TAB_SPACING) {
        tab_info[0].text_canvas.x = text_canvas.x + text_canvas.w + TAB_SPACING;
    }
    if(!TAB_INIT) {
        TAB_INIT = 1;
    }
    for(int i=1;i<tab_info_size+1;i++) {
        mplayer->menu->textures[MPLAYER_TAB_TEXTURE][i-1] = mplayer_rendertab(mplayer, &tab_info[i-1]);
        if(i+1 > 1) {
            int tab_x = tab_info[i-1].text_canvas.x + tab_info[i-1].text_canvas.w + TAB_SPACING;
            tab_info[i].text_canvas.x = tab_x;
        }
        if(tab_info[i-1].active) {
            mplayer_renderactive_tab(mplayer, &tab_info[i-1]);
        }
        SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_TAB_TEXTURE][i-1], NULL,
            &tab_info[i-1].text_canvas);
        mplayer->menu->texture_canvases[MPLAYER_TAB_TEXTURE][i-1] = tab_info[i-1].text_canvas;
    }
    //printf("3\n");
    if(active_tab == SONGS_TAB) {
        /*SDL_Texture* text_texture = mplayer_rendertext(mplayer, &text_info[1]);
        mplayer->menu->textures[MPLAYER_TEXT_TEXTURE][1] = text_texture;
        text_info[1].text_canvas.x = (WIDTH - text_info[1].text_canvas.w)/2,
        text_info[1].text_canvas.y = (HEIGHT - text_info[1].text_canvas.h)/2;
        mplayer->menu->texture_canvases[MPLAYER_TEXT_TEXTURE][1] = text_info[1].text_canvas;
        SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_TEXT_TEXTURE][1], NULL,
            &text_info[1].text_canvas);*/
    } else if(active_tab == ALBUMS_TAB) {
        /*SDL_Texture* text_texture = mplayer_rendertext(mplayer, &text_info[2]);
        mplayer->menu->textures[MPLAYER_TEXT_TEXTURE][1] = text_texture;
        text_info[2].text_canvas.x = (WIDTH - text_info[2].text_canvas.w)/2,
        text_info[2].text_canvas.y = (HEIGHT - text_info[2].text_canvas.h)/2;
        mplayer->menu->texture_canvases[MPLAYER_TEXT_TEXTURE][1] = text_info[2].text_canvas;
        SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_TEXT_TEXTURE][1], NULL,
            &text_info[2].text_canvas);*/
    }
    prev_tab = active_tab;

    // Create settings button on screen
    setting_iconbtn.btn_canvas.x = WIDTH - setting_iconbtn.btn_canvas.w - 2;
    mplayer->menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][MTOTALBTN_COUNT-1] = setting_iconbtn.btn_canvas;
    SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][MTOTALBTN_COUNT-1], NULL,
        &setting_iconbtn.btn_canvas);
    //printf("4\n");
    /* Create songs box*/
    mplayer_createsongs_box(mplayer);
    /* Create music bar */
    mplayer_createmusicbar(mplayer);

    // TODO: Make music playable
    text_info_t text = {20, NULL, white, {songs_box.x + 2, songs_box.y + 1}};
    SDL_Rect outer_canvas = text.text_canvas;
    /*  get the size of a character so we can determine the maximum amount of textures
        we can render with in the songs box
    */
    TTF_SetFontSize(mplayer->font, 20);
    TTF_SizeText(mplayer->font, "A", &text.text_canvas.w, &text.text_canvas.h);
    //TODO: FIx bug below
    size_t max_textures = songs_box.h / (text.text_canvas.h + 25);
    //printf("5\n");
    for(size_t i=0;i<max_textures;i++) {
        //printf("i.%d, %s\n", i, mplayer->music_list[i].music_name);
        text.text = mplayer->music_list[i].music_name;
        if(!text.text) break;
        SDL_Texture* text_texture = mplayer_rendertext(mplayer, &text);
        outer_canvas.h = text.text_canvas.h + 22, outer_canvas.w = WIDTH - scrollbar.w;
        text.text_canvas.x = outer_canvas.x + ((outer_canvas.h - text.text_canvas.h) / 2),
        text.text_canvas.y = outer_canvas.y + ((outer_canvas.h - text.text_canvas.h) / 2);
        mplayer_menuadd_canvas(mplayer, outer_canvas);
        SDL_SetRenderDrawColor(mplayer->renderer, 0x3B, 0x35, 0x61, 0xff);
        SDL_RenderDrawRect(mplayer->renderer, &mplayer->menu->canvases[mplayer->menu->canvas_count-1]);
        SDL_RenderFillRect(mplayer->renderer, &mplayer->menu->canvases[mplayer->menu->canvas_count-1]);
        mplayer_menu_appendtext(mplayer, text);
        mplayer_addmenu_texture(mplayer, MPLAYER_TEXT_TEXTURE);
        mplayer_menuplace_texture(mplayer, MPLAYER_TEXT_TEXTURE, text_texture, text.text_canvas);
        SDL_RenderCopy(mplayer->renderer, text_texture, NULL,
            &mplayer->menu->texture_canvases[MPLAYER_TEXT_TEXTURE][mplayer->menu->texture_sizes[MPLAYER_TEXT_TEXTURE]-1]);
        text.text_canvas.y += text.text_canvas.h + 22;
        outer_canvas.y += text.text_canvas.h + 25;
    }

    // create the scroll bar
    scrollbar.y = (HEIGHT - scrollbar.h)/2, scrollbar.x = WIDTH - (scrollbar.w+10);
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(white));
    SDL_RenderFillRect(mplayer->renderer, &scrollbar);
    SDL_RenderDrawRect(mplayer->renderer, &scrollbar);
    //printf("6\n");
    // Present all rendered objects on the screen
    SDL_RenderPresent(mplayer->renderer);
    mplayer_destroytextures(mplayer->menu->textures[MPLAYER_TEXT_TEXTURE], text_info_size-1);
    mplayer_destroytextures(mplayer->menu->textures[MPLAYER_TAB_TEXTURE], tab_info_size);
    mplayer_menu_freetext(mplayer, MPLAYER_DEFAULT_MENU);
}

void mplayer_settingmenu(mplayer_t* mplayer) {
    SDL_Rect* canvas = NULL;
    int btn_id = 0;
    mplayer_set_window_title(mplayer, SETTING_TITLE);
    mplayer_set_window_color(mplayer->renderer, setting_wincolor);
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
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_DEFAULT);
                return;
            }
        }
    }

    // create buttons, text and display on the screen
    for(size_t i=0;i<setting_textinfo_size;i++) {
        mplayer->menu->textures[MPLAYER_TEXT_TEXTURE][i] = mplayer_rendertext(mplayer, &setting_textinfo[i]);
    }

    // set the canvas x position of the text "Go Back To Home" beside the back button 
    canvas = &setting_textinfo[0].text_canvas;
    canvas->x = setting_btns[0].btn_canvas.x + setting_btns[0].btn_canvas.w + 5;

    for(size_t i=1;i<setting_textinfo_size;i++) {
        setting_textinfo[i].text_canvas.y = setting_textinfo[i-1].text_canvas.y + setting_textinfo[i-1].text_canvas.h;
    }

    // render each texture on its particular canvas
    for(size_t i=0;i<setting_textinfo_size;i++) {
        mplayer->menu->texture_canvases[MPLAYER_TEXT_TEXTURE][i] = setting_textinfo[i].text_canvas;
        SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_TEXT_TEXTURE][i], NULL,
            &mplayer->menu->texture_canvases[MPLAYER_TEXT_TEXTURE][i]);
    }

    // render buttons on screen
    for(size_t i=0;i<SETTINGSBTN_COUNT;i++) {
        SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][i], NULL,
            &mplayer->menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][i]);
    }
    SDL_RenderPresent(mplayer->renderer);
    mplayer_destroytextures(mplayer->menu->textures[MPLAYER_TEXT_TEXTURE],
        mplayer->menu->texture_sizes[MPLAYER_TEXT_TEXTURE]);
    mplayer_menu_freetext(mplayer, MPLAYER_SETTINGS_MENU);
}