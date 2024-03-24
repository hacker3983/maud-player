#include "music_player.h"

SDL_Rect *playbtn_canvas = NULL, *playbtn_listcanvas = NULL,
        *prevbtn_canvas = NULL, *skipbtn_canvas = NULL,
        *pausebtn_canvas = NULL, *repeatoffbtn_canvas = NULL,
        *repeatallbtn_canvas = NULL, *repeatonebtn_canvas = NULL,
        *shufflebtn_canvas = NULL;
void mplayer_init() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    setlocale(LC_ALL,"it_IT.UTF-16"); 
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
    mplayer->music_font = mplayer_openfont(MUSIC_FONTFILE, MUSIC_FONTSIZE);
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
    mplayer->cursors[MPLAYER_CURSOR_TYPEABLE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
    mplayer->music_hover = false;
    mplayer->music_id = 0, mplayer->prevmusic_id = 0, mplayer->playid = 0, mplayer->repeat_id = MUSIC_REPEATALLBTN;
    mplayer->music_renderpos = 0;
    mplayer->music_renderinit = false;
    mplayer->tick_count = 0;
    mplayer->music_playing = false;
    mplayer->musicsearchcursor_blink = false;
    mplayer->scroll = false;
    mplayer->scroll_type = 0;
    mplayer->progress_bar.w = 0, mplayer->progress_bar.x = 0;
    mplayer->progress_bar.h = 0, mplayer->progress_bar.y = 0;
    mplayer->progress_count.w = 0, mplayer->progress_count.h = 0;
    mplayer->progress_count.x = 0, mplayer->progress_count.y = 0;
    mplayer->music_searchbar.w = 0, mplayer->music_searchbar.x = 0;
    mplayer->music_searchbar.h = 0, mplayer->music_searchbar.y = 0;
    mplayer->progressbar_clicked = false, mplayer->musicsearchbar_clicked = false;
    mplayer->musicsearchbar_data = NULL;
    mplayer->musicsearchbar_datalen = 0;
    mplayer->current_music = NULL, mplayer->prev_music = NULL,
    mplayer->music_list = NULL;

    // create music information
    mplayer_getmusicpath_info(mplayer);
    if(mplayer->musinfo.locations == NULL) {
        #ifdef _WIN32
        wchar_t* location = NULL;
        char* username = getenv("USERNAME"), *location_str = NULL, root_path[4] = {0};
        location_str = calloc(16+strlen(username), sizeof(char));
        mplayer_getroot_path(root_path);
        strcpy(location_str, root_path);
        strcat(location_str, "Users\\");
        strcat(location_str, username);
        strcat(location_str, "\\Music");
        location = mplayer_stringtowide(location_str);
        mplayer_addmusic_location(mplayer, location);
        //mplayer_getmusicpath_info(mplayer);
        free(location_str); location_str = NULL;
        free(location); location = NULL;
        // TODO: Linux
        #else
        char* home = getenv("HOME"), *location = NULL;
        location = calloc(strlen(home) + 7, sizeof(char));
        strcat(location, home);
        strcat(location, "/Music");
        mplayer_addmusic_location(mplayer, location);
        free(home); home = NULL;
        free(location); location = NULL;
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
            break;
        case MPLAYER_CURSOR_POINTER:
            SDL_SetCursor(mplayer->cursors[cursor_type]);
            break;
        case MPLAYER_CURSOR_TYPEABLE:
            SDL_SetCursor(mplayer->cursors[cursor_type]);
            break;
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
    return false;
}

bool mplayer_music_hover(mplayer_t* mplayer) {
    SDL_Rect canvas = {0};
    mplayer->mouse_x = mplayer->e.motion.x, mplayer->mouse_y = mplayer->e.motion.y;
    // ensure that the music list has been initialized with the loaded musics
    if(!mplayer->music_list) {
        return false;
    }
    for(size_t i=mplayer->music_renderpos;i<mplayer->music_count;i++) {
        canvas = mplayer->music_list[i].outer_canvas;
        if(mplayer->music_list[i].render &&
            (mplayer->e.motion.x <= canvas.x + canvas.w && mplayer->e.motion.x >= canvas.x) &&
            (mplayer->e.motion.y <= canvas.y + canvas.h && mplayer->e.motion.y >= canvas.y)) {
            mplayer->prevmusic_id = mplayer->music_id;
            mplayer->music_list[mplayer->prevmusic_id].hover = false;
            mplayer->music_id = i; mplayer->music_list[i].hover = true; return true;
        }
    }
    mplayer->music_list[mplayer->music_id].hover = false;
    mplayer->music_hover = false;
    return false;
}

bool mplayer_songsbox_hover(mplayer_t* mplayer) {
    if((mplayer->mouse_x <= songs_box.x + songs_box.w && mplayer->mouse_x >= songs_box.x) &&
        (mplayer->mouse_y <= songs_box.y + songs_box.h && mplayer->mouse_y >= songs_box.y)) {
            return true;
    }
    return false;
}

bool mplayer_musiclist_playbutton_hover(mplayer_t* mplayer) {
    int mouse_x = mplayer->mouse_x, mouse_y = mplayer->mouse_y;
    SDL_Rect canvas = music_listplaybtn.btn_canvas;
    if((mouse_x <= canvas.x + canvas.w && mouse_x >= canvas.x) &&
        (mouse_y <= canvas.y + canvas.h && mouse_y >= canvas.y)) {
        return true;
    }
    return false;
}

bool mplayer_checkbox_hovered(mplayer_t* mplayer) {
    int mouse_x = mplayer->mouse_x, mouse_y = mplayer->mouse_y;
    if((mouse_x <= checkbox_size.x + checkbox_size.w && mouse_x >= checkbox_size.x) &&
        (mouse_y <= checkbox_size.y + checkbox_size.h && mouse_y >= checkbox_size.y)) {
            return true;
    }
    return false;
}

bool mplayer_progressbar_hover(mplayer_t* mplayer) {
    int mouse_x = mplayer->mouse_x, mouse_y = mplayer->mouse_y;
    if((mouse_x <= mplayer->progress_bar.x + mplayer->progress_bar.w && mouse_x >= mplayer->progress_bar.x) &&
        (mouse_y <= mplayer->progress_bar.y + mplayer->progress_bar.h && mouse_y >= mplayer->progress_bar.y)) {
            return true;
    }
    return false;
}

bool mplayer_searchbar_hover(mplayer_t* mplayer) {
    int mouse_x = mplayer->mouse_x, mouse_y = mplayer->mouse_y;
    if((mouse_x <= mplayer->music_searchbar.x + mplayer->music_searchbar.w && mouse_x >= mplayer->music_searchbar.x) &&
        (mouse_y <= mplayer->music_searchbar.y + mplayer->music_searchbar.h && mouse_y >= mplayer->music_searchbar.y)) {
            return true;
    }
    return false;
}

wchar_t* mplayer_stringtowide(const char* string) {
    size_t wstr_len = mbstowcs(NULL, string, 0)+1; // get the length of the string in wide characters
    size_t str_len = strlen(string);
    if(wstr_len == -1) {
        wstr_len = str_len;
    }
    wchar_t* wstring = calloc(wstr_len+1, sizeof(wchar_t));
    size_t ret = mbstowcs(wstring, string, wstr_len);
    if(ret == -1) {
        wchar_t wc = 0;
        for(size_t i=0;i<wstr_len;i++) {
            mbtowc(&wc, &string[i], 1);
            wstring[i] = wc;
        }
    }
    return wstring;
}

char* mplayer_widetostring(wchar_t* wstring) {
    size_t len_wstr = wcstombs(NULL, wstring, 0);
    char* string = calloc(len_wstr+1, sizeof(char));
    wcstombs(string, wstring, len_wstr);
    return string;
}

Uint16* mplayer_widetouint16(wchar_t* wstring) {
    size_t wstring_length = wcslen(wstring);
    Uint16* uint_string = calloc(wstring_length+1, sizeof(Uint16));
    for(size_t i=0;i<wstring_length;i++) {
        uint_string[i] = (Uint16)wstring[i];
    }
    return uint_string;
}

Uint16* mplayer_stringtouint16(char* string) {
    size_t strsize = strlen(string);
    Uint16* uint16_string = calloc(strsize, sizeof(Uint16));
    for(size_t i = 0; i < strsize; i++) {
        uint16_string[i] = string[i];
    }
    return uint16_string;
}

SDL_Texture* mplayer_rendertext(mplayer_t* mplayer, TTF_Font* font, text_info_t* text_info) {
    SDL_Rect text_canvas = text_info->text_canvas;
    TTF_SetFontSize(font, text_info->font_size);
    TTF_SizeText(font, text_info->text, &text_canvas.w, &text_canvas.h);
    SDL_Surface* surface = TTF_RenderText_Blended(font, text_info->text, text_info->text_color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(mplayer->renderer, surface);
    SDL_FreeSurface(surface);
    text_info->text_canvas = text_canvas;
    return texture;
}

SDL_Texture* mplayer_renderunicode_text(mplayer_t* mplayer, TTF_Font* font, text_info_t* utext_info) {
    SDL_Rect utext_canvas = utext_info->text_canvas;
    TTF_SetFontSize(font, utext_info->font_size);
    #ifdef _WIN32
    Uint16* uint16_string = mplayer_widetouint16(utext_info->utext);
    #else
    Uint16* uint16_string = mplayer_stringtouint16(utext_info->text);
    #endif
    TTF_SizeUNICODE(font, uint16_string, &utext_canvas.w, &utext_canvas.h);
    SDL_Surface* surface = TTF_RenderUNICODE_Blended(font, uint16_string, utext_info->text_color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(mplayer->renderer, surface);
    SDL_FreeSurface(surface);
    free(uint16_string); uint16_string = NULL;
    utext_info->text_canvas = utext_canvas;
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
    TTF_CloseFont(mplayer->font);
    TTF_CloseFont(mplayer->music_font);

    // free whatever data the user types into the searchbar
    free(mplayer->musicsearchbar_data); mplayer->musicsearchbar_data = NULL;
    mplayer->musicsearchbar_datalen = 0;

    // Free resources used by program
    mplayer_freemusic_info(mplayer);
     // destroys music player graphical utilities
    SDL_DestroyRenderer(mplayer->renderer);
    SDL_DestroyWindow(mplayer->window);

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
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
}

void mplayer_createmusicbar(mplayer_t* mplayer/*, SDL_Texture* musicbtn_textures[]*/) {
    playbtn_canvas = &music_btns[MUSIC_PLAYBTN].btn_canvas,
    skipbtn_canvas = &music_btns[MUSIC_SKIPBTN].btn_canvas,
    prevbtn_canvas = &music_btns[MUSIC_PREVBTN].btn_canvas;
    shufflebtn_canvas = &music_btns[MUSIC_SHUFFLEBTN].btn_canvas;
    pausebtn_canvas = &music_btns[MUSIC_PAUSEBTN].btn_canvas;
    repeatoffbtn_canvas = &music_btns[MUSIC_REPEATOFFBTN].btn_canvas;
    repeatallbtn_canvas = &music_btns[MUSIC_REPEATALLBTN].btn_canvas;
    repeatonebtn_canvas = &music_btns[MUSIC_REPEATONEBTN].btn_canvas;
    
    // create music bar
    music_status.y = HEIGHT - music_status.h, music_status.w = WIDTH;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(music_statusbar_color));
    SDL_RenderFillRect(mplayer->renderer, &music_status);
    SDL_RenderDrawRect(mplayer->renderer, &music_status);

    // place the play, previous and skip button in the middle of the music status
    playbtn_canvas->x = (WIDTH - playbtn_canvas->w)/2, playbtn_canvas->y = HEIGHT -
        (playbtn_canvas->h + (playbtn_canvas->h/2));
    
    pausebtn_canvas->x = playbtn_canvas->x, pausebtn_canvas->y = playbtn_canvas->y;

    /*prevbtn_canvas->x = ((WIDTH - prevbtn_canvas->w)/4), prevbtn_canvas->y = HEIGHT - (prevbtn_canvas->h +
       (prevbtn_canvas->h/2));*/
    shufflebtn_canvas->x = ((WIDTH - shufflebtn_canvas->w)/2) - shufflebtn_canvas->w - 60,
    shufflebtn_canvas->y = HEIGHT - (shufflebtn_canvas->h + (shufflebtn_canvas->h/2));
    prevbtn_canvas->x = ((WIDTH - prevbtn_canvas->w)/2) - prevbtn_canvas->w - 10, prevbtn_canvas->y = HEIGHT - (prevbtn_canvas->h +
       (prevbtn_canvas->h/2));

    /*skipbtn_canvas->x = (WIDTH - (skipbtn_canvas->w))/2 + (WIDTH - skipbtn_canvas->w)/4, skipbtn_canvas->y = HEIGHT -
        (skipbtn_canvas->h + (skipbtn_canvas->h/2));*/
    skipbtn_canvas->x = ((WIDTH - (skipbtn_canvas->w))/2) + skipbtn_canvas->w + 10, skipbtn_canvas->y = HEIGHT -
        (skipbtn_canvas->h + (skipbtn_canvas->h/2));

    repeatallbtn_canvas->x = skipbtn_canvas->x + skipbtn_canvas->w + 10,
    repeatallbtn_canvas->y = skipbtn_canvas->y;

    repeatonebtn_canvas->x = repeatallbtn_canvas->x,
    repeatonebtn_canvas->y = repeatallbtn_canvas->y;

    repeatoffbtn_canvas->x = repeatonebtn_canvas->x,
    repeatoffbtn_canvas->y = repeatonebtn_canvas->y;
    SDL_Texture* target_texture = NULL;
    // render all buttons on music status bar
    for(int i=0;i<MUSICBTN_COUNT;i++) {
        mplayer->menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][i] = music_btns[i].btn_canvas;
        if(i == MUSIC_PLAYBTN || i == MUSIC_PAUSEBTN) {
            target_texture = mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][MUSIC_PLAYBTN];
            if(Mix_PlayingMusic() && !Mix_PausedMusic()) {
                target_texture = mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][MUSIC_PAUSEBTN];
            }
            if(music_btns[i].clicked) {
                if(Mix_PlayingMusic() && Mix_PausedMusic() && music_btns[MUSIC_PLAYBTN].clicked) {
                    Mix_ResumeMusic();
                } else if(Mix_PlayingMusic() && !Mix_PausedMusic() && music_btns[MUSIC_PLAYBTN].clicked) {
                    Mix_PauseMusic();
                }
                music_btns[i].clicked = false;
            }
            SDL_RenderCopy(mplayer->renderer, target_texture, NULL, &music_btns[i].btn_canvas);
            continue;
        } else if(i == MUSIC_REPEATALLBTN || i == MUSIC_REPEATONEBTN || i == MUSIC_REPEATOFFBTN) {
            target_texture = mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][mplayer->repeat_id];
            SDL_RenderCopy(mplayer->renderer, target_texture, NULL, &music_btns[mplayer->repeat_id].btn_canvas);
            continue;
        }
        SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][i],
            NULL, &music_btns[i].btn_canvas);
    }
}

SDL_TimerCallback draw_cursorblink(void* m) {
    mplayer_t* mplayer = (mplayer_t*)m;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(white));
    SDL_RenderDrawRect(mplayer->renderer, &mplayer->music_searchbar_cursor);
    SDL_RenderFillRect(mplayer->renderer, &mplayer->music_searchbar_cursor);
}

void mplayer_createsearch_bar(mplayer_t* mplayer) {
    // Calculate the music search bar positions and render the search bar to the screen
    mplayer->music_searchbar.w = WIDTH - 100, mplayer->music_searchbar.h = 50;
    mplayer->music_searchbar.x = (int)roundf((float)(WIDTH - mplayer->music_searchbar.w) / (float)2);
    mplayer->music_searchbar.y = tab_info[0].text_canvas.y + tab_info[0].text_canvas.h + (UNDERLINE_THICKNESS + 5);
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(music_searchbar_color));
    SDL_RenderDrawRect(mplayer->renderer, &mplayer->music_searchbar);

    /* Check whether the music search data is empty and the user hasn't clicked the search bar.
       if that condition is true then we render the placeholder within the music search bar
       otherwise 
    */
    text_info_t *placeholder = &text_info[1], searchbar_data = {0};
    SDL_Texture* texture = NULL;
    if(!mplayer->musicsearchbar_data && !mplayer->musicsearchbar_clicked) {
        texture = mplayer_rendertext(mplayer, mplayer->music_font, placeholder);
        placeholder->text_canvas.y = mplayer->music_searchbar.y + ((mplayer->music_searchbar.h -
            placeholder->text_canvas.h) / 2),
        placeholder->text_canvas.x = mplayer->music_searchbar.x + ((mplayer->music_searchbar.w -
            placeholder->text_canvas.w) / 2);
        SDL_RenderCopy(mplayer->renderer, texture, NULL, &placeholder->text_canvas);
        SDL_DestroyTexture(texture);
        return;
    }
    
    if(mplayer->musicsearchbar_data) {
        searchbar_data.text = mplayer->musicsearchbar_data;
        searchbar_data.font_size = 18;
        searchbar_data.text_color = white;
        searchbar_data.text_canvas.x = mplayer->music_searchbar.x + 1,
        searchbar_data.utext = NULL;
        texture = mplayer_rendertext(mplayer, mplayer->music_font, &searchbar_data);
        searchbar_data.text_canvas.y = mplayer->music_searchbar.y + ((mplayer->music_searchbar.h -
            searchbar_data.text_canvas.h) / 2);
        SDL_RenderCopy(mplayer->renderer, texture, NULL, &searchbar_data.text_canvas);
        SDL_DestroyTexture(texture);
    }
    if(mplayer->musicsearchbar_clicked) {
        mplayer->music_searchbar_cursor.w = 1, mplayer->music_searchbar_cursor.h = mplayer->music_searchbar.h / 2;
        mplayer->music_searchbar_cursor.x = (mplayer->music_searchbar.x + 2) + searchbar_data.text_canvas.w,
        mplayer->music_searchbar_cursor.y = mplayer->music_searchbar.y + ((mplayer->music_searchbar.h - 
            mplayer->music_searchbar_cursor.h)/2);
        if(mplayer->musicsearchcursor_blink) {
            mplayer->musicsearchcursor_blink = false;
        } else {
            SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(white));
            SDL_RenderDrawRect(mplayer->renderer, &mplayer->music_searchbar_cursor);
            SDL_RenderFillRect(mplayer->renderer, &mplayer->music_searchbar_cursor);
            mplayer->musicsearchcursor_blink = true;
        }
    } else {
        printf("We lost focus in the searchbar\n");
    }

}

void mplayer_createsongs_box(mplayer_t* mplayer) {
    songs_box.y = mplayer->music_searchbar.y + mplayer->music_searchbar.h + 5/*tab_info[0].text_canvas.y + tab_info[0].text_canvas.h + (UNDERLINE_THICKNESS + 5)*/;
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

void mplayer_drawcheckbox(mplayer_t* mplayer, mcheckbox_t* checkbox_info) {
    int mid_x1 = checkbox_info->checkbox_canvas.x,
        mid_y1 = checkbox_info->checkbox_canvas.y,
        mid_x2 = checkbox_info->checkbox_canvas.x + checkbox_info->checkbox_canvas.w,
        mid_y2 = checkbox_info->checkbox_canvas.y + checkbox_info->checkbox_canvas.h;
    SDL_Color box_color = checkbox_info->box_color, tick_color = checkbox_info->tk_color,
        fill_color = checkbox_info->fill_color;
    SDL_SetRenderDrawColor(mplayer->renderer, box_color.r, box_color.g, box_color.b, box_color.a);
    SDL_RenderDrawRect(mplayer->renderer, &checkbox_info->checkbox_canvas);
    if(checkbox_info->fill) {
        SDL_SetRenderDrawColor(mplayer->renderer, fill_color.r, fill_color.g, fill_color.b, fill_color.a);
        SDL_RenderFillRect(mplayer->renderer, &checkbox_info->checkbox_canvas);
    }
    if(checkbox_info->tick) {
        SDL_SetRenderDrawColor(mplayer->renderer, tick_color.r, tick_color.g, tick_color.b, tick_color.a);
        // draw longest line for tick
        int y = 10, y2 = 6;
        for(int i=0;i<2;i++) {
            int k = mid_x1+10;
            for(int j=mid_y2-y;j>=mid_y1+5;j--) {
                SDL_RenderDrawPoint(mplayer->renderer, k, j);
                k++;
            }
            y--;
        }
        y = 10;
        for(int i=0;i<2;i++) {
            int k = mid_x1+10;
            for(int j=mid_y2-y;j>=(mid_y2-y2)-y;j--) {
                SDL_RenderDrawPoint(mplayer->renderer, k, j);
                k--;
            }
            y--; y2--;
        }
    }
}

void mplayer_drawmusic_checkbox(mplayer_t* mplayer, SDL_Color box_color, SDL_Color fill_color,
    bool fill, SDL_Color tick_color, bool check) {
    size_t music_id = mplayer->music_id;
    mcheckbox_t checkbox_info = {0};
    checkbox_info.checkbox_canvas = checkbox_size;
    checkbox_info.box_color = box_color;
    checkbox_info.tk_color = tick_color;
    checkbox_info.tick = check;
    checkbox_info.fill = fill;
    mplayer_drawcheckbox(mplayer, &checkbox_info);
}

void mplayer_displaymusic_status(mplayer_t* mplayer, mtime_t curr_duration, mtime_t full_duration) {
    text_info_t duration_texts[2] = {0};
    SDL_Texture* textures[2];
    duration_texts[0].text = calloc(9, sizeof(char));
    duration_texts[0].text_canvas.x = music_status.x + 1, duration_texts[0].text_canvas.y = music_status.y + 10;
    duration_texts[0].text_color = white;

    duration_texts[1].text = calloc(9, sizeof(char));
    duration_texts[1].text_canvas.x = music_status.x + music_status.w,
    duration_texts[1].text_canvas.y = music_status.y + 10;
    duration_texts[1].text_color = white;

    sprintf(duration_texts[0].text, "%02d:%02d:%02d", curr_duration.hrs, curr_duration.mins, curr_duration.secs);
    sprintf(duration_texts[1].text, "%02d:%02d:%02d", full_duration.hrs, full_duration.mins, full_duration.secs);
    for(int i=0;i<2;i++) {
        duration_texts[i].font_size = 14;
        textures[i] = mplayer_rendertext(mplayer, mplayer->music_font, &duration_texts[i]);
    }
    mplayer->progress_bar.w = WIDTH - (duration_texts[0].text_canvas.w * 2) - 20,
    mplayer->progress_bar.x = duration_texts[0].text_canvas.x + duration_texts[0].text_canvas.w + 6;
    mplayer->progress_bar.y = music_status.y+10, mplayer->progress_bar.h = duration_texts[0].text_canvas.h;
    duration_texts[1].text_canvas.x = mplayer->progress_bar.x + mplayer->progress_bar.w + 6;
    for(int i=0;i<2;i++) {
        free(duration_texts[i].text); duration_texts[i].text = NULL;
        SDL_RenderCopy(mplayer->renderer, textures[i], NULL, &duration_texts[i].text_canvas);
        SDL_DestroyTexture(textures[i]);
    }

    if(Mix_PlayingMusic()) {
        text_info_t music_name = {18, NULL, NULL, white, {20, 0, 0, 0}};
        music_name.text_canvas.y = prevbtn_canvas->y - 10;
        #ifdef _WIN32
        music_name.utext = mplayer->current_music->music_name;
        #else
        music_name.text = mplayer->current_music->music_name;
        #endif
        SDL_Texture* texture = mplayer_renderunicode_text(mplayer, mplayer->music_font, &music_name);
        SDL_RenderCopy(mplayer->renderer, texture, NULL, &music_name.text_canvas);
        SDL_DestroyTexture(texture); texture = NULL;
    }
}

void mplayer_renderprogress_bar(mplayer_t* mplayer, SDL_Color bar_color, SDL_Color line_color, 
    double curr_durationsecs, double full_durationsecs) {
    int percentage = round((double)(curr_durationsecs) / (double)(full_durationsecs) * (double)100);
    int percentageof = (int)round(((double)percentage / (double)100 * (double)mplayer->progress_bar.w));

    mplayer->progress_count.x = mplayer->progress_bar.x;
    mplayer->progress_count.y = mplayer->progress_bar.y;
    
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(bar_color));
    SDL_RenderDrawRect(mplayer->renderer, &mplayer->progress_bar);
    SDL_RenderFillRect(mplayer->renderer, &mplayer->progress_bar);

    if(Mix_PlayingMusic()) {
        mplayer->progress_count.w = percentageof;
        mplayer->progress_count.h = mplayer->progress_bar.h;
        if(mplayer->progressbar_clicked) {
            // Determine what percentage of the widtth of the progress bar was clicked
            int seek_position = mplayer->mouse_x - mplayer->progress_bar.x;
            percentage = round((double)seek_position / (double)mplayer->progress_bar.w * (double)100);
            mplayer->progress_count.w = seek_position;
            /* Calculate the new duration based on the portion of the progress bar that was clicked
               and then set the new music position
            */
            curr_durationsecs = round(((double)percentage / (double)100) * (double)full_durationsecs);
            Mix_SetMusicPosition(curr_durationsecs);
            mplayer->progressbar_clicked = false;
        }
        SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(line_color));
        SDL_RenderDrawRect(mplayer->renderer, &mplayer->progress_count);
        SDL_RenderFillRect(mplayer->renderer, &mplayer->progress_count);
    }
}

void mplayer_displayprogression_control(mplayer_t* mplayer) {
    mtime_t curr_duration = {0}, full_duration = {0};
    double full_durationsecs = 0, curr_durationsecs = 0;
    int progress = 0;
    if(Mix_PlayingMusic()) {
        curr_durationsecs = Mix_GetMusicPosition(mplayer->current_music->music);
        full_durationsecs = mplayer->current_music->music_durationsecs;
        full_duration = mplayer->current_music->music_duration;
        curr_duration = mplayer_music_gettime(curr_durationsecs);
        progress = curr_durationsecs / full_durationsecs * 100;
    }
    if(Mix_PlayingMusic() && !Mix_PausedMusic()) {
        #ifdef _WIN32
        printf("Playing %ls %02d:%02d:%02ds of %02d:%02d:%02ds %d%% completed\n", mplayer->current_music->music_name,
            curr_duration.hrs, curr_duration.mins, curr_duration.secs,
            full_duration.hrs, full_duration.mins, full_duration.secs, progress);
        #else
        printf("Playing %s %02d:%02d:%02ds of %02d:%02d:%02ds %d%% completed\n", mplayer->current_music->music_name,
            curr_duration.hrs, curr_duration.mins, curr_duration.secs,
            full_duration.hrs, full_duration.mins, full_duration.secs, progress);
        #endif
    }
    switch(mplayer->repeat_id) {
        case MUSIC_REPEATALLBTN:
            if(mplayer->playid < mplayer->music_count && mplayer->current_music && !Mix_PlayingMusic()) {
                // play the next music whenever one is completed
                mplayer->playid++;
                mplayer->playid %= mplayer->music_count;
                mplayer->current_music = &mplayer->music_list[mplayer->playid];
                Mix_PlayMusic(mplayer->current_music->music, 1);
                break;
            }
            break;
        case MUSIC_REPEATONEBTN:
            if(!Mix_PlayingMusic() && mplayer->current_music) {
                Mix_PlayMusic(mplayer->current_music->music, 1);
            }
            break;
        case MUSIC_REPEATOFFBTN:
            if(mplayer->playid < mplayer->music_count && mplayer->current_music && !Mix_PlayingMusic()) {
                // play the next music whenever one is completed
                mplayer->playid++;
                mplayer->playid %= mplayer->music_count;
                mplayer->current_music = &mplayer->music_list[mplayer->playid];
                Mix_PlayMusic(mplayer->current_music->music, 1);
                if(mplayer->playid == 0) {
                    // This prevents the music from playing automatically whenever we reach the end of the music list
                    Mix_PauseMusic();
                }
                break;
            }
            break;
    }
    mplayer_displaymusic_status(mplayer, curr_duration, full_duration);
    SDL_Color progressbar_color = {0x00, 0x00, 0x00, 0x00}, progress_linecolor = {0xFF, 0xFF, 0x00, 0xFF};
    mplayer_renderprogress_bar(mplayer, progressbar_color, progress_linecolor, curr_durationsecs, full_durationsecs);
}

void mplayer_rendersongs(mplayer_t* mplayer) {
    int cursor = MPLAYER_CURSOR_DEFAULT;
    playbtn_listcanvas = &music_btns[MUSIC_LISTPLAYBTN].btn_canvas;
    text_info_t utext = {14, NULL, NULL, white, {songs_box.x + 2, songs_box.y + 1}};
    int default_w = 0, default_h = 0;
    SDL_Rect outer_canvas = utext.text_canvas;
    /*  get the size of a character so we can determine the maximum amount of textures
        we can render with in the songs box
    */
    TTF_SetFontSize(mplayer->music_font, utext.font_size);
    #ifdef _WIN32
    TTF_SizeUNICODE(mplayer->music_font, (Uint16*)L"A", &utext.text_canvas.w, &utext.text_canvas.h);
    #else
    TTF_SizeUNICODE(mplayer->music_font, (Uint16*)"A", &utext.text_canvas.w, &utext.text_canvas.h);
    #endif
    default_w = utext.text_canvas.w, default_h = utext.text_canvas.h + 22;
    // calculation for the scrollability
    size_t max_textures = (size_t)roundf((float)songs_box.h / ((float)utext.text_canvas.h + (float)25)),
        render_count = 0;
    bool render_stop = 0, fit = 0;
    size_t trender_count = 0;
    // Get the starting render position so that if we scroll we can update the positions for the musics that
    // are being renderered on the screen
    SDL_Rect prev_canvas = mplayer->music_list[mplayer->music_renderpos].outer_canvas;
    prev_canvas.x = songs_box.x + 2, prev_canvas.y = songs_box.y + 1;
    mplayer->music_list[mplayer->music_renderpos].outer_canvas.y = songs_box.y + 1;
    for(size_t i=0;i<mplayer->music_count;i++) {
        utext = mplayer->music_list[i].text_info;
        outer_canvas = mplayer->music_list[i].outer_canvas;
        utext.utext = mplayer->music_list[i].music_name;
        if(!utext.utext || !mplayer->music_list[i].render) continue;
        if(fit) {
            // whenever the last texture can fit within the songs box then we render a portion of the next music
            TTF_SetFontSize(mplayer->music_font, utext.font_size);
            #ifdef _WIN32
            TTF_SizeUNICODE(mplayer->music_font, (Uint16*)mplayer->music_list[i].text_info.utext,
                &default_w, &default_h);
            #else
            TTF_SizeUNICODE(mplayer->music_font, (Uint16*)mplayer->music_list[i].text_info.text,
                &default_w, &default_h);
            #endif
            mplayer->music_list[i].outer_canvas.h = default_h - ((outer_canvas.y + default_h) - (songs_box.y + songs_box.h) + 1);
            mplayer->music_list[i].text_info.text_canvas.y = outer_canvas.y + ((outer_canvas.h - utext.text_canvas.h) / 2);
            fit = false;
        }
        if(mplayer->scroll) {
            //printf("mplayer->music_count - mplayer music_renderpos : %ld ? max_textures: %ld, i: %ld\n",
            //(mplayer->music_count - mplayer->music_renderpos), max_textures, i);
            printf("Mplayer->music_renderpos is %ld\n", mplayer->music_renderpos);
            if(mplayer->scroll_type == MPLAYERSCROLL_DOWN && (mplayer->music_count - mplayer->music_renderpos) >= max_textures) {
                //printf("DOWN\n");
                // whenever we scroll down and textures can hold on the screen we increase the render position
                if(mplayer->music_list[i].outer_canvas.h >= 3) {
                    mplayer->music_list[i].outer_canvas.h -= 3;
                    printf("Here\n");
                    if(mplayer->music_list[i].outer_canvas.h <= 3) {
                        printf("ok\n");
                        mplayer->music_list[i].render = false;
                        mplayer->music_renderpos++;
                        mplayer->music_renderpos %= mplayer->music_count;
                    }
                }
            } else if(mplayer->scroll_type == MPLAYERSCROLL_UP && mplayer->music_renderpos >= 0) {
                if(mplayer->music_list[mplayer->music_renderpos].outer_canvas.h < default_h) {
                    mplayer->music_list[mplayer->music_renderpos].outer_canvas.h += 3;
                    mplayer->music_list[mplayer->music_renderpos].render = true;
                } else if(mplayer->music_list[mplayer->music_renderpos].outer_canvas.h >= default_h &&
                    mplayer->music_renderpos > 0) {
                    mplayer->music_renderpos--;
                    mplayer->music_list[mplayer->music_renderpos].render = true;

                }
                if(mplayer->music_list[i].outer_canvas.h >= default_h &&
                    mplayer->music_renderpos > 0) {
                    mplayer->music_renderpos--;
                    printf("here multiple times mplayer->music_renderpos: %ld\n", mplayer->music_renderpos);
                }
            }
            mplayer->scroll = false;
        } else if(render_count > max_textures) {
            // whenever the render_count is greater than the max texture and a texture cannot hold at the end
            // of the screen we exit the loop
            break;
        } else if(render_count == max_textures-1) {
            // Calculate the original height for the current
            TTF_SetFontSize(mplayer->music_font, utext.font_size);
            #ifdef _WIN32
            TTF_SizeUNICODE(mplayer->music_font, (Uint16*)mplayer->music_list[i].text_info.utext,
                &default_w, &default_h);
            #else
            TTF_SizeUNICODE(mplayer->music_font, (Uint16*)mplayer->music_list[i].text_info.text,
                &default_w, &default_h);
            #endif
            default_h += 22;
            // whenever the last texture cannot fit inside the songs box then we stop rendering more music
            if((outer_canvas.y + default_h) > (songs_box.y + songs_box.h)) {
                mplayer->music_list[i].outer_canvas.h = default_h - ((outer_canvas.y + default_h) -
                    (songs_box.y + songs_box.h) + 1);
                render_stop = true;
            } else {
                fit = true;
            }
        } else if(render_count > 0 && render_count < max_textures-1 && outer_canvas.h < default_h) {
            TTF_SetFontSize(mplayer->music_font, utext.font_size);
            #ifdef _WIN32
            TTF_SizeUNICODE(mplayer->music_font, (Uint16*)mplayer->music_list[i].text_info.utext,
                &default_w, &default_h);
            #else
            TTF_SizeUNICODE(mplayer->music_font, (Uint16*)mplayer->music_list[i].text_info.text,
                &default_w, &default_h);
            #endif
            default_h += 22;
            mplayer->music_list[i].outer_canvas.h = default_h;
        }
        //printf("The render positions is %ld\n", mplayer->music_renderpos);
        mplayer->music_list[i].outer_canvas.x = prev_canvas.x,
        mplayer->music_list[i].outer_canvas.y = prev_canvas.y;
        prev_canvas.y += mplayer->music_list[i].outer_canvas.h + 3;
        mplayer->music_list[i].text_info.text_canvas.y = mplayer->music_list[i].outer_canvas.y +
            ((mplayer->music_list[i].outer_canvas.h - mplayer->music_list[i].text_info.text_canvas.h) / 2);
        mplayer->music_list[i].outer_canvas.w = WIDTH - scrollbar.w;
        SDL_SetRenderDrawColor(mplayer->renderer, 0x3B, 0x35, 0x61, 0xff);
        SDL_RenderDrawRect(mplayer->renderer, &mplayer->music_list[i].outer_canvas);
        SDL_RenderFillRect(mplayer->renderer, &mplayer->music_list[i].outer_canvas);
        // calculate the y position for the next musics outercanvas

        SDL_Color box_color = {0xff, 0xff, 0xff, 0xff}, tick_color = {0x00, 0xff, 0x00, 0xff},
                fill_color = {0xFF, 0xA5, 0x00, 0xff};
        checkbox_size.x = outer_canvas.x+5, checkbox_size.y = utext.text_canvas.y-5;
        checkbox_size.h = outer_canvas.h-10;

        music_listplaybtn.btn_canvas.w = 30, music_listplaybtn.btn_canvas.h = outer_canvas.h - 10;
        music_listplaybtn.btn_canvas.x = (checkbox_size.x + checkbox_size.w) + 20,
        music_listplaybtn.btn_canvas.y = checkbox_size.y;

        int mouse_x = mplayer->mouse_x, mouse_y = mplayer->mouse_y;
        SDL_Rect hoverbg_canvas = {0};
        // check if we hover over the play button and we haven't ticked any checkbox
        if(mplayer_musiclist_playbutton_hover(mplayer) && !mplayer->tick_count) {
                hoverbg_canvas = music_listplaybtn.btn_canvas;
                hoverbg_canvas.x -= 5, hoverbg_canvas.w += 5;
                hoverbg_canvas.h = outer_canvas.h, hoverbg_canvas.y = outer_canvas.y;
                SDL_SetRenderDrawColor(mplayer->renderer, 0x00, 0x00, 0x00, 0x00);
                SDL_RenderDrawRect(mplayer->renderer, &hoverbg_canvas);
                SDL_RenderFillRect(mplayer->renderer, &hoverbg_canvas);
        }

        if(music_btns[MUSIC_PREVBTN].clicked && mplayer->playid >= 0) {
            // This prevents a music from going to the previous music when none was playing before
            if(Mix_PlayingMusic()) {
                if(mplayer->playid) {
                    mplayer->playid--;
                }
                mplayer->current_music = &mplayer->music_list[mplayer->playid];
                Mix_HaltMusic();
                if(!Mix_PausedMusic()) {
                    Mix_PlayMusic(mplayer->current_music->music, 1);
                } else {
                    Mix_PlayMusic(mplayer->current_music->music, 1);
                    Mix_PauseMusic();
                }
            }
            music_btns[MUSIC_PREVBTN].clicked = false;
        } else if(music_btns[MUSIC_SKIPBTN].clicked && mplayer->playid <= mplayer->music_count) {
            // This prevents a music from skipping to another when none was being played before
            if(Mix_PlayingMusic()) {
                mplayer->playid++;
                mplayer->playid %= mplayer->music_count;
                mplayer->current_music = &mplayer->music_list[mplayer->playid];
                Mix_HaltMusic();
                if(!Mix_PausedMusic()) {
                    Mix_PlayMusic(mplayer->current_music->music, 1);
                } else {
                    Mix_PlayMusic(mplayer->current_music->music, 1);
                    Mix_PauseMusic();
                }
            }
            music_btns[MUSIC_SKIPBTN].clicked = false;
        } else if(mplayer->music_list[i].hover) {
            // check if the mouse is hovered over the music
            if(mplayer_checkbox_hovered(mplayer)) {
                if(mplayer->music_list[i].clicked) {
                    // if we are in the position of the checkbox and we clicked it
                    switch(mplayer->music_list[i].checkbox_ticked) {
                        case false:
                            /* whenever the checkbox isn't already ticked then we set checkbox as ticked
                               and the fill color for it as true
                            */
                            mplayer->music_list[i].fill = true;
                            mplayer->music_list[i].checkbox_ticked = true;
                            mplayer->tick_count++;
                            break;
                        case true:
                            /* whenever the checkbox is already ticked then we set the chekcbox as not ticked
                               and the fill equal to false
                            */
                            mplayer->music_list[i].fill = false;
                            mplayer->music_list[i].checkbox_ticked = false;
                            mplayer->tick_count--;
                            break;
                    }
                    mplayer->music_list[i].clicked = false;
                } else {
                    /* if we just hover over the checkbox without clicking it then we set that checkbox
                       fill equal to true
                    */
                    mplayer->music_list[i].fill = true;
                }
                cursor = MPLAYER_CURSOR_POINTER;
                mplayer_setcursor(mplayer, cursor);
            } else if(mplayer_musiclist_playbutton_hover(mplayer) && !mplayer->tick_count) {
                if(mplayer->music_list[i].clicked) {
                    /* whenever we hover over the playbutton on the music
                       we determine if we should restart the current playing music or play a new music
                    */
                    if(Mix_PlayingMusic() && mplayer->playid == i) {
                        Mix_SetMusicPosition(0);
                        if(Mix_PausedMusic()) {
                            Mix_ResumeMusic();
                            mplayer->music_list[i].music_playing = true;
                        }
                    } else {
                        mplayer->current_music = &mplayer->music_list[i];
                        if(Mix_PlayingMusic()) {
                            Mix_HaltMusic();
                        }
                        mplayer->playid = i;
                        mplayer->music_list[i].music_playing = true;
                        Mix_PlayMusic(mplayer->current_music->music, 1);
                    }
                    mplayer->music_list[i].clicked = false;
                }
                cursor = MPLAYER_CURSOR_POINTER;
                mplayer_setcursor(mplayer, cursor);
            } else if(mplayer->music_list[i].clicked) {
                // whenever we click the music without clicking any of its elements
                // we set clicked equal to false to prevent it from performing any action that we do not want
                mplayer->music_list[i].clicked = false;
            }
            mplayer->menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][music_listplaybtn.texture_idx] =
                music_listplaybtn.btn_canvas;
            if(!mplayer->tick_count) {
                // if we haven't ticked any checkbox then we render the music list play button to the screen
                utext.text_canvas.x += music_listplaybtn.btn_canvas.w + 20;
                SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE]
                    [music_listplaybtn.texture_idx], NULL, &music_listplaybtn.btn_canvas);
            }
            mplayer_setcursor(mplayer, cursor);
        }

        if(mplayer->tick_count) {
            if(!mplayer->music_list[i].checkbox_ticked && !mplayer->music_list[i].hover) {
                mplayer->music_list[i].fill = false;
            }
            mplayer_drawmusic_checkbox(mplayer, box_color, fill_color, mplayer->music_list[i].fill, tick_color,
                mplayer->music_list[i].checkbox_ticked);
        } else if (mplayer->music_list[i].hover) {
            if(mplayer_checkbox_hovered(mplayer)) {
                mplayer_drawmusic_checkbox(mplayer, box_color, fill_color, mplayer->music_list[i].fill, tick_color,
                    mplayer->music_list[i].checkbox_ticked);
            } else if(!mplayer_checkbox_hovered(mplayer)) {
                mplayer_drawmusic_checkbox(mplayer, box_color, fill_color, false, tick_color, false);
            }
        }
        SDL_RenderCopy(mplayer->renderer, mplayer->music_list[i].text_texture, NULL,
            &utext.text_canvas);
        render_count++;
        if(render_stop) { break; }
    }
}

void mplayer_setup_menu(mplayer_t* mplayer) {
    mplayer_menu_t* menu = &mplayer->menus[mplayer->menu_opt];
    mplayer->menu = menu;
    if(mplayer->menu_opt == MPLAYER_DEFAULT_MENU && menu->textures[MPLAYER_TEXT_TEXTURE] == NULL) {
        mplayer_createmenu_texture(mplayer, MPLAYER_TEXT_TEXTURE, text_info_size-1);
        mplayer_createmenu_texture(mplayer, MPLAYER_TAB_TEXTURE, tab_info_size);
        mplayer_createmenu_texture(mplayer, MPLAYER_BUTTON_TEXTURE, MUSICBTN_COUNT/*MTOTALBTN_COUNT*/);
        // Load Button Textures and Canvas's
        for(int i=0;i<MUSICBTN_COUNT;i++) {
            /*printf("music_btns[i].imgbtn_path: %s, x: %d, y: %d\n", music_btns[i].imgbtn_path,
                music_btns[i].btn_canvas.x, music_btns[i].btn_canvas.y);*/
            menu->textures[MPLAYER_BUTTON_TEXTURE][i] = IMG_LoadTexture(mplayer->renderer, music_btns[i].imgbtn_path);
            menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][i] = music_btns[i].btn_canvas;
            music_btns[i].texture_idx = i;
        }

        // TODO: REPLACE WITH mplayer_menuplacetexture function

        // Add a texture for the add folder icon button
        mplayer_addmenu_texture(mplayer, MPLAYER_BUTTON_TEXTURE);
        music_addfolderbtn.texture_idx = menu->texture_sizes[MPLAYER_BUTTON_TEXTURE]-1;
        menu->textures[MPLAYER_BUTTON_TEXTURE][music_addfolderbtn.texture_idx] = IMG_LoadTexture(mplayer->renderer,
            music_addfolderbtn.imgbtn_path);
        menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][music_addfolderbtn.texture_idx] = music_addfolderbtn.btn_canvas;

        // Add a texture for settings icon button
        mplayer_addmenu_texture(mplayer, MPLAYER_BUTTON_TEXTURE);
        setting_iconbtn.texture_idx = menu->texture_sizes[MPLAYER_BUTTON_TEXTURE]-1;
        menu->textures[MPLAYER_BUTTON_TEXTURE][setting_iconbtn.texture_idx] = IMG_LoadTexture(mplayer->renderer,
            setting_iconbtn.imgbtn_path);
        menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][setting_iconbtn.texture_idx] = setting_iconbtn.btn_canvas;

        // music list play button icon
        mplayer_addmenu_texture(mplayer, MPLAYER_BUTTON_TEXTURE);
        music_listplaybtn.texture_idx = menu->texture_sizes[MPLAYER_BUTTON_TEXTURE]-1;
        menu->textures[MPLAYER_BUTTON_TEXTURE][music_listplaybtn.texture_idx] = IMG_LoadTexture(mplayer->renderer,
            music_listplaybtn.imgbtn_path);
        menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][music_listplaybtn.texture_idx] = music_listplaybtn.btn_canvas; 
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
    while(!mplayer->quit) {
        if(mplayer->menu_opt == MPLAYER_DEFAULT_MENU) {
            mplayer_defaultmenu(mplayer);
        } else if(mplayer->menu_opt == MPLAYER_SETTINGS_MENU) {
            mplayer_settingmenu(mplayer);
        }
    }
}

void mplayer_defaultmenu(mplayer_t* mplayer) {
    bool checkbox_ticked = false;
    bool *music_clicked = &mplayer->music_clicked;
    int tab_hoverid = 0;
    int music_id = 0;
    mplayer_set_window_title(mplayer, WINDOW_TITLE);
    while(SDL_PollEvent(&mplayer->e)) {
        if(mplayer->e.type == SDL_QUIT) {
                mplayer->quit = 1; break;
        }  else if(mplayer->e.type == SDL_TEXTINPUT) {
            if(mplayer->musicsearchbar_clicked) {
                if(!mplayer->musicsearchbar_data) {
                    mplayer->musicsearchbar_datalen = strlen(mplayer->e.text.text);
                    mplayer->musicsearchbar_data = calloc(mplayer->musicsearchbar_datalen, sizeof(char));
                } else {
                    mplayer->musicsearchbar_datalen += strlen(mplayer->e.text.text);
                    mplayer->musicsearchbar_data = realloc(mplayer->musicsearchbar_data,
                        (mplayer->musicsearchbar_datalen + 1) * sizeof(char));
                }
                strcat(mplayer->musicsearchbar_data, mplayer->e.text.text);
                mplayer->musicsearchbar_data[mplayer->musicsearchbar_datalen] = 0;
            }
        } else if(mplayer->e.type == SDL_KEYDOWN) {
            if(mplayer->e.key.keysym.scancode == SDL_SCANCODE_BACKSPACE) {
                if(mplayer->musicsearchbar_clicked && mplayer->musicsearchbar_datalen > 0) {
                    mplayer->musicsearchbar_datalen--;
                    mplayer->musicsearchbar_data[mplayer->musicsearchbar_datalen] = 0;
                    if(mplayer->musicsearchbar_datalen == 0) {
                        free(mplayer->musicsearchbar_data); mplayer->musicsearchbar_data = NULL;
                    } else {
                        char* newsearchbar_data = calloc(mplayer->musicsearchbar_datalen+1, sizeof(char));
                        newsearchbar_data[mplayer->musicsearchbar_datalen] = 0;
                        strcpy(newsearchbar_data, mplayer->musicsearchbar_data);
                        free(mplayer->musicsearchbar_data); mplayer->musicsearchbar_data = newsearchbar_data;
                    }
                }
            }
        } else if(mplayer->e.type == SDL_MOUSEMOTION) {
            if(mplayer_ibutton_hover(mplayer, setting_iconbtn)) {
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
                setting_iconbtn.hover = true;
            } else if(mplayer_ibutton_hover(mplayer, music_addfolderbtn)) {
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
                music_addfolderbtn.hover = true;
            } else if(mplayer_tabs_hover(mplayer, tab_info, &tab_hoverid, tab_info_size) && TAB_INIT) {
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
            } else if(mplayer_music_hover(mplayer)) {
                music_id = mplayer->music_id;
            } else if(mplayer_progressbar_hover(mplayer) && Mix_PlayingMusic()) {
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
            } else if(mplayer_searchbar_hover(mplayer)) {
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_TYPEABLE);
            } else {
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_DEFAULT);
                setting_iconbtn.hover = false;
            }
            mplayer->mouse_x = mplayer->e.motion.x, mplayer->mouse_y = mplayer->e.motion.y;
        } else if(mplayer->e.type == SDL_MOUSEWHEEL) {
            if(mplayer->e.wheel.y > 0) {
                mplayer->scroll_type = MPLAYERSCROLL_UP;
            } else if(mplayer->e.wheel.y < 0) {
                mplayer->scroll_type = MPLAYERSCROLL_DOWN;
            } else if(mplayer->e.wheel.x > 0) {
                mplayer->scroll_type = MPLAYERSCROLL_RIGHT;
            } else if(mplayer->e.wheel.x < 0) {
                mplayer->scroll_type = MPLAYERSCROLL_LEFT;
            }
            mplayer->scroll = true;
        } else if(Mix_PlayingMusic() && mplayer->e.type == SDL_KEYUP) {
            if(mplayer->e.key.keysym.sym == SDLK_SPACE && !mplayer->musicsearchbar_clicked)
                if(Mix_PausedMusic()) {
                    Mix_ResumeMusic();
                } else {
                    Mix_PauseMusic();
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
            } else if(mplayer_ibutton_hover(mplayer, music_addfolderbtn)) {
                mplayer_browsefolder(mplayer);
            } else if(mplayer_ibutton_hover(mplayer, music_btns[MUSIC_PLAYBTN])) {
                // since the pause and play button will be in the same position we
                // can just check if we clicked in the position for the play button
                switch(music_btns[MUSIC_PLAYBTN].clicked) {
                    case true:
                        // if we clicked in the position for the play button already
                        // that means we clicked on the pause btn
                        music_btns[MUSIC_PLAYBTN].clicked = false;
                        music_btns[MUSIC_PAUSEBTN].clicked = true;
                        break;
                    case false:
                        // if play button wasn't already clicked then that means we clicked on the play button
                        music_btns[MUSIC_PAUSEBTN].clicked = false;
                        music_btns[MUSIC_PLAYBTN].clicked = true;
                        break;

                }
            } else if(mplayer_ibutton_hover(mplayer, music_btns[MUSIC_PREVBTN])) {
                music_btns[MUSIC_PREVBTN].clicked = true;
            } else if(mplayer_ibutton_hover(mplayer, music_btns[MUSIC_SKIPBTN])) {
                music_btns[MUSIC_SKIPBTN].clicked = true;
            } else if(mplayer_ibutton_hover(mplayer, music_btns[MUSIC_REPEATALLBTN])) {
                if(mplayer->repeat_id == MUSIC_REPEATOFFBTN) {
                    mplayer->repeat_id = MUSIC_REPEATALLBTN;
                    break;
                }
                music_btns[mplayer->repeat_id++].clicked = true;
            } else if(mplayer_music_hover(mplayer)) {
                mplayer->mouse_x = mplayer->e.button.x;
                mplayer->mouse_y = mplayer->e.button.y;
                mplayer->music_list[mplayer->music_id].clicked = true;
            } else if(mplayer_progressbar_hover(mplayer)) {
                if(Mix_PlayingMusic()) {
                    mplayer->mouse_x = mplayer->e.motion.x, mplayer->mouse_y = mplayer->e.motion.y;
                    mplayer->progressbar_clicked = true;
                }
            } else {
                *music_clicked = false;
            }

            /* Check if we have clicked in the music search bar. if the condition is true we set clicked equal to false
               Otherwise if the we haven't clicked the music search bar but another region of the screen then we lose
               focus in the search bar
            */
            if(mplayer_searchbar_hover(mplayer)) {
                mplayer->musicsearchbar_clicked = true;
            } else {
                mplayer->musicsearchbar_clicked = false;
            }
        }
    }
    SDL_GetWindowSize(mplayer->window, &WIDTH, &HEIGHT);
    mplayer_set_window_color(mplayer->renderer, window_color);
    mplayer->menu->textures[MPLAYER_TEXT_TEXTURE][0] = mplayer_rendertext(mplayer, mplayer->font, &text_info[0]);
    mplayer->menu->texture_canvases[MPLAYER_TEXT_TEXTURE][0] = text_info[0].text_canvas;
    SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_TEXT_TEXTURE][0], NULL,
            &text_info[0].text_canvas);
    SDL_Rect tab_canvas = tab_info[0].text_canvas, text_canvas = text_info[0].text_canvas;
    tab_info[0].text_canvas.x = text_canvas.x + text_canvas.w + TAB_SPACING;
    if(!TAB_INIT) { TAB_INIT = 1; }
    for(int i=0;i<tab_info_size;i++) {
        mplayer->menu->textures[MPLAYER_TAB_TEXTURE][i] = mplayer_rendertab(mplayer, &tab_info[i]);
        if(i > 0) {
            int tab_x = tab_info[i-1].text_canvas.x + tab_info[i-1].text_canvas.w + TAB_SPACING;
            tab_info[i].text_canvas.x = tab_x;
        }
        (tab_info[i].active) ?  mplayer_renderactive_tab(mplayer, &tab_info[i]) : 0;
        SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_TAB_TEXTURE][i], NULL,
            &tab_info[i].text_canvas);
        mplayer->menu->texture_canvases[MPLAYER_TAB_TEXTURE][i] = tab_info[i].text_canvas;
    }

    // Create add folder button on screen
    music_addfolderbtn.btn_canvas.x = WIDTH - (setting_iconbtn.btn_canvas.w * 2) - 4;
    mplayer->menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][music_addfolderbtn.texture_idx] =
        music_addfolderbtn.btn_canvas;
    SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][music_addfolderbtn.texture_idx],
        NULL, &music_addfolderbtn.btn_canvas);

    // Create settings button on screen
    setting_iconbtn.btn_canvas.x = WIDTH - setting_iconbtn.btn_canvas.w - 2;
    mplayer->menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][setting_iconbtn.texture_idx] =
        setting_iconbtn.btn_canvas;
    SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][setting_iconbtn.texture_idx],
        NULL, &setting_iconbtn.btn_canvas);
    /* Create the search bar for searching for music */
    mplayer_createsearch_bar(mplayer);
    /* Create songs box*/
    mplayer_createsongs_box(mplayer);
    /* Create music bar */
    mplayer_createmusicbar(mplayer);
    if(!mplayer->music_list) {
        mplayer_loadmusics(mplayer);
    }
    mplayer_displayprogression_control(mplayer);
    if(active_tab == SONGS_TAB) {
        mplayer_rendersongs(mplayer);
    } else if(active_tab == ALBUMS_TAB) {
    }
    prev_tab = active_tab;

    // create the scroll bar
    scrollbar.y = songs_box.y + 2, scrollbar.x = WIDTH - (scrollbar.w+10);
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(white));
    SDL_RenderFillRect(mplayer->renderer, &scrollbar);
    SDL_RenderDrawRect(mplayer->renderer, &scrollbar);

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
        mplayer->menu->textures[MPLAYER_TEXT_TEXTURE][i] = mplayer_rendertext(mplayer, mplayer->font, &setting_textinfo[i]);
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