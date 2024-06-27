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
    mplayer->music_id = 0, mplayer->prevmusic_id = 0, mplayer->playid = 0, mplayer->repeat_id = MUSIC_REPEATALLBTN,
    mplayer->searchid = 0;
    mplayer->music_renderpos = 0;
    mplayer->music_searchrenderpos = 0;
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
    mplayer->musicsearchbar_data = NULL, mplayer->musicsearchbar_datainfo = (text_info_t){0};
    mplayer->musicsearchbar_datalen = 0, mplayer->musicsearchbar_datarenderpos = 0;
    mplayer->current_musicsearch_query = NULL, mplayer->current_musicsearch_querylen = 0;
    mplayer->musicsearchcursor_relpos = 0;
    mplayer->current_music = NULL, mplayer->prev_music = NULL,
    mplayer->music_list = NULL, mplayer->music_searchresult = NULL,
    mplayer->music_populatedsearch_result = NULL;
    mplayer->music_searchresult_indices = NULL;
    mplayer->music_count = 0, mplayer->music_searchresult_count = 0;
    mplayer->music_searchresult_indicescount = 0;
    mplayer->musicpending_removalcount = 0;
    mplayer->populate_index = 0, mplayer->music_populatedresult_count = 0;
    mplayer->match_maxrenderpos = 0;
    mplayer->location_count = 0; mplayer->total_filecount = 0;
    mplayer->music_locationremoved = false, mplayer->music_locationadded = false;
    mplayer->music_newsearch = false;
    mplayer->mouse_x = 0, mplayer->mouse_y = 0;
    mplayer->mouse_buttondown = false;
    mplayer->mouse_clicked = false;
    mplayer->music_searchresult_ready = false;
    mplayer->start_search = false;
    mplayer->blink_timeout = 0;
    mplayer->search_thread = 0;
    mplayer->searchthread_created = false;
    mplayer->searchthread_creationtimer = 0;

    // create music information
    mplayer_getmusicpath_info(mplayer);
    if(mplayer->locations == NULL) {
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
        free(location_str); location_str = NULL;
        free(location); location = NULL;
        #else
        char* home = getenv("HOME"), *location = NULL;
        location = calloc(strlen(home) + 7, sizeof(char));
        strcat(location, home);
        strcat(location, "/Music");
        mplayer_addmusic_location(mplayer, location);
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

bool mplayer_rect_hover(mplayer_t* mplayer, SDL_Rect rect) {
    if((mplayer->mouse_x <= rect.x + rect.w && mplayer->mouse_x >= rect.x) &&
        (mplayer->mouse_y <= rect.y + rect.h && mplayer->mouse_y >= rect.y)) {
        return true;
    }
    return false;
}

bool mplayer_ibutton_hover(mplayer_t* mplayer, ibtn_t button) {
    int x = button.btn_canvas.x, y = button.btn_canvas.y,
        w = button.btn_canvas.w, h = button.btn_canvas.h;
    if((mplayer->mouse_x <= x + h && mplayer->mouse_x >= x) &&
        (mplayer->mouse_y <= y + h && mplayer->mouse_y >= y)) {
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

bool mplayer_music_hover(mplayer_t* mplayer, size_t index) {
    SDL_Rect canvas = mplayer->music_list[index].outer_canvas;
    if(mplayer->musicsearchbar_data && mplayer->music_searchresult_count) {
        canvas = mplayer->music_searchresult[index].outer_canvas;   
    }
    if((mplayer->mouse_x <= canvas.x + canvas.w && mplayer->mouse_x >= canvas.x) &&
        (mplayer->mouse_y <= canvas.y + canvas.h && mplayer->mouse_y >= canvas.y)) {
            mplayer->prevmusic_id = mplayer->music_id;
            mplayer->music_id = index; return true;
    }
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

bool mplayer_music_searchsubstr(mplayer_t* mplayer, size_t search_index) {
    /* Create a copy of the music search data and the music_name so that we do not modify the contents
       of the original and convert them to lowercase / uppercase so we can make our match incase sensitive
    */
    bool match = false;
    #ifdef _WIN32
    size_t music_namelen = wcslen(mplayer->music_list[search_index].music_name),
           music_querylen = mplayer->current_musicsearch_querylen;
    wchar_t *music_name = calloc(music_namelen+1, sizeof(wchar_t));
    if(!music_name) {
        printf("calloc failed for music name allocation\n");
    }
    //printf("mplayer->musicsearchbar_data is %s\n", mplayer->musicsearchbar_data);
    //printf("mplayer->musicsearchbar_datalen: %zu\n", mplayer->musicsearchbar_datalen);
    wchar_t *music_query = mplayer_stringtowide(mplayer->current_musicsearch_query);
    if(!music_query) {
        printf("Memory allocation failed for music_query\n");
    }
    wcsncpy(music_name, mplayer->music_list[search_index].music_name, music_namelen);
    //printf("mplayer_music_searchsubstr(): music_name: %ls, music_query: %ls\n", music_name, music_query);
    // Compare the lowercase versions to see if we get a match
    wchar_t *match_string = (wchar_t*)mplayer_strcasestr(music_name, music_query);
    #else
    size_t music_namelen = strlen(mplayer->music_list[search_index].music_name),
           music_querylen = mplayer->current_musicsearch_querylen;
    char *music_name = calloc(music_namelen+1, sizeof(char)),
         *music_query = calloc(music_querylen+1, sizeof(char));
    strncpy(music_name, mplayer->music_list[search_index].music_name, music_namelen);
    strncpy(music_query, mplayer->current_musicsearch_query, music_querylen);
    char *match_string = (char*)mplayer_strcasestr(music_name, music_query);
    #endif
    if(match_string) {
        match = true;
    }
    free(music_query); free(music_name); music_query = NULL; music_name = NULL;
    free(match_string); match_string = NULL;
    return match;
}

void mplayer_search_music(mplayer_t* mplayer) {
    if(!mplayer->musicsearchbar_data) {
        free(mplayer->music_searchresult_indices); mplayer->music_searchresult_indices = NULL;
        mplayer->music_searchresult_indicescount = 0;
        return;
    }
    if(!mplayer->music_newsearch) {
        return;
    }
    mplayer->searchid = 0, mplayer->music_searchrenderpos = 0;
    free(mplayer->music_searchresult_indices); mplayer->music_searchresult_indices = NULL;
    mplayer->music_searchresult_indicescount = 0;
    size_t *music_searchresult_indices = NULL, music_searchresult_indicescount = 0;
    for(size_t i=0;i<mplayer->music_count;i++) {
        if(mplayer_music_searchsubstr(mplayer, i)) {
            mplayer->music_list[i].outer_canvas.h = mplayer->music_list[i].text_info.text_canvas.h + 22;
            mplayer->music_list[i].search_match = true;
            mplayer->music_list[i].search_render = true;
            if(!music_searchresult_indices) {
                music_searchresult_indices = (size_t*)calloc(1, sizeof(size_t));
            } else {
                music_searchresult_indices = (size_t*)realloc(music_searchresult_indices,
                    (music_searchresult_indicescount+1) * sizeof(size_t));
            }
            music_searchresult_indices[music_searchresult_indicescount++] = i;
            continue;
        }
        mplayer->music_list[i].search_match = false;
    }
    mplayer->music_searchresult_indices = music_searchresult_indices;
    mplayer->music_searchresult_indicescount = music_searchresult_indicescount;
}

void mplayer_populate_searchresults(mplayer_t* mplayer) {
    if(!mplayer->musicsearchbar_data && mplayer->music_searchresult) {
        mplayer_freemusic_searchresult(&mplayer->music_searchresult, &mplayer->music_searchresult_count);
        mplayer->music_searchresult_ready = false;
        return;
    }
    if(!mplayer->music_searchresult_ready) {
        // check if we are searching for something new and results are present
        // as long as results are present we destroy the previous search results
        // or if the music search bar is empty and contains data we free the search results
        if(mplayer->music_searchresult && mplayer->music_newsearch) {
            printf("Freeing previous seach result\n");
            mplayer_freemusic_searchresult(&mplayer->music_searchresult, &mplayer->music_searchresult_count);
        } else if(mplayer->music_searchresult && mplayer->update_searchresults) {
            mplayer_freemusic_searchresult(&mplayer->music_searchresult, &mplayer->music_searchresult_count);
            printf("Freeing previous seach result\n");
        }
        return;
    }
    if(mplayer->music_searchresult && mplayer->music_newsearch) {
        printf("Freeing previous search results\n");
        mplayer_freemusic_searchresult(&mplayer->music_searchresult, &mplayer->music_searchresult_count);
        mplayer->music_newsearch = false;
    } else if(mplayer->update_searchresults && mplayer->music_searchresult) {
        printf("Freeing previous search results\n");
        mplayer_freemusic_searchresult(&mplayer->music_searchresult, &mplayer->music_searchresult_count);
        mplayer->update_searchresults = false;
    }
    if(!mplayer->music_searchresult_indices) {
        printf("mplayer->music_searchresultindices = NULL\n");
    }
    printf("mplayer->music_searchresult_indicescount: %zu\n", mplayer->music_searchresult_indicescount);
    if(!mplayer->music_searchresult) {
        printf("mplayer->music_searchresult is NULL so allocating memory\n");
        mplayer->populate_index = 0;
        mplayer->music_searchresult = calloc(mplayer->music_searchresult_indicescount, sizeof(music_t));
        mplayer->music_searchresult_count = mplayer->music_searchresult_indicescount;
    }
    printf("mplayer->music_searchresult is not NULL\n");
    printf("mplayer->populate_index is %zu\n", mplayer->populate_index);
    printf("mplayer->music_searchresult_count is %zu\n", mplayer->music_searchresult_count);
    if(mplayer->populate_index < mplayer->music_searchresult_count) {
        size_t music_index = mplayer->music_searchresult_indices[mplayer->populate_index];
        printf("in mplayer_populate_searchresults(...): at line %d\n", __LINE__);
        mplayer_copymusicinfo_fromsearchindex(mplayer, music_index,
            &mplayer->music_searchresult[mplayer->populate_index]);
        printf("in mplayer_populate_searchresults(...): at line %d\n", __LINE__);
        mplayer->match_maxrenderpos = mplayer->populate_index;
        printf("in mplayer_populate_searchresults(...): at line %d\n", __LINE__);
        mplayer->populate_index++;
        printf("in mplayer_populate_searchresults(...): at line %d\n", __LINE__);
        return;
    }
    mplayer->populate_index = 0;
    mplayer->music_searchresult_ready = false;
}

void* mplayer_searchthread(void* arg) {
    mplayer_t* mplayer = (mplayer_t*)arg;
    // Ensure that the search bar has been clicked and contains data before locing the mutex,
    // performing search operations, and unlocking the mutex
    printf("Inside mplayer_searchthread():\n");
    printf("mplayer->start_search: %d\n", mplayer->start_search);
    printf("mplayer->update_searchresults: %d\n", mplayer->update_searchresults);
    if(mplayer->start_search || mplayer->update_searchresults) {
        mplayer->current_musicsearch_query = mplayer_dupstr(mplayer->musicsearchbar_data,
                                                mplayer->musicsearchbar_datalen);
        mplayer->current_musicsearch_querylen = mplayer->musicsearchbar_datalen;
        printf("Search query: %s\n", mplayer->current_musicsearch_query);
        mplayer_search_music(mplayer);
        mplayer->music_searchresult_ready = true;
        mplayer->start_search = false;
        free(mplayer->current_musicsearch_query); mplayer->current_musicsearch_query = NULL;
        mplayer->current_musicsearch_querylen = 0;
    }
    return NULL;
}

char* mplayer_stringtolower(char** string, size_t wlen) {
    char* new_string = calloc(wlen+1, sizeof(char));
    for(size_t i=0;i<wlen;i++) {
        new_string[i] = tolower((*string)[i]);
    }
    free(*string); *string = new_string;
    return new_string;
}

wchar_t* mplayer_widetolower(wchar_t** wstring, size_t len) {
    wchar_t* new_wstring = calloc(len+1, sizeof(wchar_t));
    for(size_t i=0;i<len;i++) {
        new_wstring[i] = towlower((*wstring)[i]);
    }
    free(*wstring); *wstring = new_wstring;
    return new_wstring;
}

wchar_t* mplayer_stringtowide(const char* string) {
    if(!string) {
        printf("From mplayer string to wide(): the string give as a parameter is NULL\n");
        return NULL;
    }
    size_t wstr_len = mbstowcs(NULL, string, 0)+1; // get the length of the string in wide characters
    size_t str_len = strlen(string);
    if(wstr_len == -1) {
        printf("mbstowcs(): failed at first\n");
        wstr_len = str_len;
    }
    wchar_t* wstring = calloc(wstr_len+1, sizeof(wchar_t));
    size_t ret = mbstowcs(wstring, string, wstr_len);
    if(ret == -1) {
        printf("mbstowc(): failed again\n");
        wchar_t wc = 0;
        for(size_t i=0;i<wstr_len;i++) {
            if(mbtowc(&wc, &string[i], 1) < 0) {
                printf("Failed to convert to from string to wide as mbtowc() failed\n");
            }
            wstring[i] = wc;
        }
    }
    return wstring;
}


void* mplayer_dupstr(void* string, size_t len) {
    if(!string) {
        return NULL;
    }
    #ifdef _WIN32
    wchar_t *new_dupstr = (wchar_t*)calloc(len+1, sizeof(wchar_t));
    wcsncpy(new_dupstr, (wchar_t*)string, len);
    #else
    char *new_dupstr = (char*)calloc(len+1, sizeof(char));
    strncpy(new_dupstr, (char*)string, len);
    #endif
    return new_dupstr;
}

void* mplayer_strcasestr(void* haystack, void* needle) {
    size_t haystack_len = 0, needle_len = 0, match_substrlen = 0;
    if(!haystack) {
        return NULL;
    }
    if(!needle) {
        return NULL;
    }
    #ifdef _WIN32
    haystack_len = wcslen((wchar_t*)haystack), needle_len = wcslen((wchar_t*)needle);
    wchar_t *haystack_dup = (wchar_t*)mplayer_dupstr(haystack, haystack_len),
            *needle_dup = (wchar_t*)mplayer_dupstr(needle, needle_len);
    mplayer_widetolower(&haystack_dup, haystack_len),
    mplayer_widetolower(&needle_dup, needle_len);
    wchar_t* match_substr = wcsstr(haystack_dup, needle_dup);
    if(match_substr) {
        match_substrlen = wcslen(match_substr);
        match_substr = (wchar_t*)mplayer_dupstr(match_substr, match_substrlen);
    }
    #else
    haystack_len = strlen((char*)haystack), needle_len = strlen((char*)needle);
    char *haystack_dup = (char*)mplayer_dupstr(haystack, haystack_len),
         *needle_dup = (char*)mplayer_dupstr(needle, needle_len);
    mplayer_stringtolower(&haystack_dup, haystack_len);
    mplayer_stringtolower(&needle_dup, needle_len);
    char* match_substr = strstr(haystack_dup, needle_dup);
    if(match_substr) {
        match_substrlen = strlen(match_substr);
        match_substr = (char*)mplayer_dupstr(match_substr, match_substrlen);
    }
    #endif
    free(haystack_dup); haystack_dup = NULL;
    free(needle_dup); needle_dup = NULL;
    return match_substr;
}

char* mplayer_widetostring(wchar_t* wstring) {
    size_t len_wstr = wcstombs(NULL, wstring, 0);
    char* string = calloc(len_wstr+1, sizeof(char));
    wcstombs(string, wstring, len_wstr);
    return string;
}

Uint16* mplayer_widetouint16(wchar_t* wstring) {
    if(wstring == NULL) {
        return NULL;
    }
    size_t wstring_length = wcslen(wstring);
    Uint16* uint_string = calloc(wstring_length+1, sizeof(Uint16));
    for(size_t i=0;i<wstring_length;i++) {
        uint_string[i] = (Uint16)wstring[i];
    }
    return uint_string;
}

Uint16* mplayer_stringtouint16(char* string) {
    size_t strsize = strlen(string);
    Uint16* uint16_string = calloc(strsize+1, sizeof(Uint16));
    for(size_t i = 0; i < strsize; i++) {
        uint16_string[i] = string[i];
    }
    return uint16_string;
}

SDL_Texture* mplayer_rendertext(mplayer_t* mplayer, TTF_Font* font, text_info_t* text_info) {
    if(!text_info->text) {
        printf("mplayer_rendertext(): the text given is equal to NULL\n");
        return NULL;
    } 
    SDL_Rect text_canvas = text_info->text_canvas;
    TTF_SetFontSize(font, text_info->font_size);
    if(TTF_SizeText(font, text_info->text, &text_canvas.w, &text_canvas.h)) {
    }
    SDL_Surface* surface = TTF_RenderText_Blended(font, text_info->text, text_info->text_color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(mplayer->renderer, surface);
    SDL_FreeSurface(surface);
    text_info->text_canvas = text_canvas;
    return texture;
}

int mplayer_get_textsize(TTF_Font* font, int font_size, text_info_t* utext_info) {
    int ret = 0;
    if(utext_info->utext) {
        #ifdef _WIN32
        Uint16* uint16_string = mplayer_widetouint16(utext_info->utext);
        ret = TTF_SizeUNICODE(font, utext_info->utext, &utext_info->text_canvas.w, &utext_info->text_canvas.h);
        free(uint16_string);
        #else
        ret = TTF_SizeUTF8(font, utext_info->utext, &utext_info->text_canvas.w, &utext_info->text_canvas.h);
        #endif
    } else if(utext_info->text) {
        ret = TTF_SizeText(font, utext_info->text, &utext_info->text_canvas.w, &utext_info->text_canvas.h);
    }
    return ret;
}

SDL_Texture* mplayer_renderunicode_text(mplayer_t* mplayer, TTF_Font* font, text_info_t* utext_info) {
    if(!utext_info->utext) {
        return NULL;
    }
    TTF_SetFontSize(font, utext_info->font_size);
    if(mplayer_get_textsize(font, utext_info->font_size, utext_info) == -1) {
        printf("mplayer_getsize(): failed because %s\n", SDL_GetError());
        return NULL;
    }
    #ifdef _WIN32
    Uint16* uint16_string = mplayer_widetouint16(utext_info->utext);
    if(!uint16_string) {
        return NULL;
    }
    SDL_Surface* surface = TTF_RenderUNICODE_Blended(font, uint16_string, utext_info->text_color);
    free(uint16_string); uint16_string = NULL;
    if(surface == NULL) {
        printf("TTF_RenderUNICODE_BLENDED(): Failed because %s\n", SDL_GetError());
        return NULL;
    }
    #else
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, utext_info->utext, utext_info->text_color);
    #endif
    SDL_Texture* texture = SDL_CreateTextureFromSurface(mplayer->renderer, surface);
    SDL_FreeSurface(surface); surface = NULL;
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
    // Close the Fonts used by the program to render text
    TTF_CloseFont(mplayer->font);
    TTF_CloseFont(mplayer->music_font);

    // Release the memory back to the system that was used to create the cursors
    for(int i=0;i<sizeof(mplayer->cursors)/sizeof(SDL_Cursor*);i++) {
        SDL_FreeCursor(mplayer->cursors[i]); mplayer->cursors[i] = NULL;
    }

    // free whatever data the user types into the searchbar
    free(mplayer->musicsearchbar_data); mplayer->musicsearchbar_data = NULL;
    mplayer->musicsearchbar_datalen = 0;

    // Free music resources used by program
    mplayer_freemusic_list(mplayer);
    mplayer_freemusicpath_info(mplayer);

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

    // Destroys music player graphical utilities
    SDL_DestroyRenderer(mplayer->renderer);
    SDL_DestroyWindow(mplayer->window);

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
        mplayer_tooltip_t musicbtn_tooltip = {
            .background_color = {0x22, 0x18, 0x1C, 0xFF},
            .text_color = {0xC5, 0xFF, 0xFD, 0xFF},
            .text = music_btns[i].tooltip_text,
            .margin_x = 10,
            .margin_y = 10,
            .x = 0,
            .y = -1,
            .element_canvas = music_btns[i].btn_canvas,
            .delay_secs = 0,
            .duration_secs = 0,
            .font = mplayer->music_font,
            .font_size = 18
        };
        mplayer->menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][i] = music_btns[i].btn_canvas;
        if(i == MUSIC_PLAYBTN || i == MUSIC_PAUSEBTN) {
            target_texture = mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][MUSIC_PLAYBTN];
            if(Mix_PlayingMusic() && !Mix_PausedMusic()) {
                target_texture = mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][MUSIC_PAUSEBTN];
                mplayer_rendertooltip(mplayer, &musicbtn_tooltip);
            } else if(Mix_PlayingMusic() && i == MUSIC_PLAYBTN) {
                mplayer_rendertooltip(mplayer, &musicbtn_tooltip);
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
            if(i == mplayer->repeat_id) {
                mplayer_rendertooltip(mplayer, &musicbtn_tooltip);
            }
            SDL_RenderCopy(mplayer->renderer, target_texture, NULL, &music_btns[mplayer->repeat_id].btn_canvas);
            continue;
        }
        if(Mix_PlayingMusic()) {
            mplayer_rendertooltip(mplayer, &musicbtn_tooltip);
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
    text_info_t *placeholder = &text_info[1], *searchbar_data = &mplayer->musicsearchbar_datainfo;
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
        int decrease_count = 0;
        if(searchbar_data->text_canvas.w > mplayer->music_searchbar.w) {
            decrease_count = searchbar_data->text_canvas.w - mplayer->music_searchbar.w;
        }
        //printf("Decrease_count: %d\n", decrease_count);
        size_t text_size = mplayer->musicsearchbar_datalen - mplayer->musicsearchbar_datarenderpos;
        size_t j = mplayer->musicsearchbar_datarenderpos;
        //printf("text_size: %zu\n", text_size);
        char* text = calloc(text_size+1, sizeof(char));
        for(size_t i=0;i<text_size;i++) {
            if(j < mplayer->musicsearchbar_datalen) {
                text[i] = mplayer->musicsearchbar_data[j++];
            }
        }
        searchbar_data->text = text;


        /*if(searchbar_data->text_canvas.w > mplayer->music_searchbar.w && mplayer->musicsearchbar_datarenderpos < mplayer->musicsearchbar_datalen) {
            free(text);
            mplayer->musicsearchbar_datarenderpos++;
            text_size = mplayer->musicsearchbar_datalen - mplayer->musicsearchbar_datarenderpos;
            size_t j = mplayer->musicsearchbar_datarenderpos;
            for(size_t i=0;i<text_size;i++) {
                if(j < mplayer->musicsearchbar_datalen) {
                    text[i] = mplayer->musicsearchbar_data[j++];
                }
            }
            searchbar_data->text = text;
            // TODO: Handle the situation of when the text goes outside of the songs box;
        }*/
        searchbar_data->font_size = 18;
        searchbar_data->text_color = white;
        searchbar_data->text_canvas.x = mplayer->music_searchbar.x + 1,
        searchbar_data->utext = NULL;
        texture = mplayer_rendertext(mplayer, mplayer->music_font, searchbar_data);
        searchbar_data->text_canvas.y = mplayer->music_searchbar.y + ((mplayer->music_searchbar.h -
            searchbar_data->text_canvas.h) / 2);
        SDL_RenderCopy(mplayer->renderer, texture, NULL, &searchbar_data->text_canvas);
        SDL_DestroyTexture(texture);
    }
    if(mplayer->musicsearchbar_clicked) {
        int char_w = 0, relpos = mplayer->musicsearchcursor_relpos;
        if(relpos > 0) {
            char* str = calloc(relpos+1, sizeof(char));
            strncpy(str, searchbar_data->text, relpos);
            TTF_SizeText(mplayer->music_font, str, &char_w, NULL);
            free(str);
        }
        mplayer->music_searchbar_cursor.w = 1, mplayer->music_searchbar_cursor.h = mplayer->music_searchbar.h / 2;
        mplayer->music_searchbar_cursor.x = mplayer->music_searchbar.x + 2 + char_w,
        mplayer->music_searchbar_cursor.y = mplayer->music_searchbar.y + ((mplayer->music_searchbar.h - mplayer->music_searchbar_cursor.h)/2);
        if(mplayer->musicsearchcursor_blink) {
            if(!mplayer->blink_timeout) {
                mplayer->blink_timeout = SDL_GetTicks() + 500;
            }
            if(SDL_TICKS_PASSED(SDL_GetTicks(), mplayer->blink_timeout)) {
                mplayer->musicsearchcursor_blink = false;
                mplayer->blink_timeout = 0;
            }
        } else {
            if(!mplayer->blink_timeout) {
                mplayer->blink_timeout = SDL_GetTicks() + 500;
            }
            SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(underline_color));
            SDL_RenderDrawRect(mplayer->renderer, &mplayer->music_searchbar_cursor);
            SDL_RenderFillRect(mplayer->renderer, &mplayer->music_searchbar_cursor);
            if(SDL_TICKS_PASSED(SDL_GetTicks(), mplayer->blink_timeout)) {
                mplayer->blink_timeout = 0;
                mplayer->musicsearchcursor_blink = true;
            }
        }
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

void mplayer_rendertooltip(mplayer_t* mplayer, mplayer_tooltip_t* tooltip) {
    // Ensure that the tooltip pops up only when the user hovers of the particular elements canvas such as a button
    // or icon on screen
    if(!mplayer_rect_hover(mplayer, tooltip->element_canvas)) {
        return;
    }
    SDL_Rect tooltip_canvas = {
        tooltip->element_canvas.x, // The horizontal position for the tooltip
        tooltip->element_canvas.y, // The vertical position for the tooltip
        tooltip->margin_x * 2, // Width of the tooltip
        tooltip->margin_y * 2  // Height of the tooltip
    };
    // Get the width and the height of the text so we can determine the tooltip box dimensions
    TTF_SetFontSize(tooltip->font, tooltip->font_size);
    int tooltip_text_w = 0, tooltip_text_h = 0;
    TTF_SizeText(tooltip->font, tooltip->text, &tooltip_text_w, &tooltip_text_h);
    tooltip_canvas.w += tooltip_text_w;
    tooltip_canvas.h += tooltip_text_h;

    if(tooltip->x < 0) {
        tooltip_canvas.x -= tooltip_canvas.w + (-tooltip->x);
    } else {
        tooltip_canvas.x += tooltip->x + tooltip->element_canvas.w;
    }

    if(tooltip->y < 0) {
        tooltip_canvas.y -= tooltip_canvas.h + (-tooltip->y);
    } else if(tooltip->y > 0) {
        tooltip_canvas.y -= tooltip_canvas.h + tooltip->y;
        printf("tooltip_canvas.y = %d\n", tooltip_canvas.y);
    }

    // TODO: IMPLEMENT BALLOON TOOLTIP and also the delay functionality
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(tooltip->background_color));
    SDL_RenderDrawRect(mplayer->renderer, &tooltip_canvas);
    SDL_RenderFillRect(mplayer->renderer, &tooltip_canvas);

    SDL_Rect tooltip_textcanvas = {
        tooltip_canvas.x + tooltip->margin_x,
        tooltip_canvas.y + tooltip->margin_y,
        tooltip_text_w,
        tooltip_text_h
    };

    SDL_Surface* tooltip_surface = TTF_RenderText_Blended(tooltip->font, tooltip->text, tooltip->text_color);
    SDL_Texture* tooltip_texture = SDL_CreateTextureFromSurface(mplayer->renderer, tooltip_surface);
    SDL_RenderCopy(mplayer->renderer, tooltip_texture, NULL, &tooltip_textcanvas);
    SDL_FreeSurface(tooltip_surface); tooltip_surface = NULL;
    SDL_DestroyTexture(tooltip_texture); tooltip_texture = NULL;
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
    //printf("curr_duration = %02d:%02d:%02d\n", curr_duration.hrs, curr_duration.mins, curr_duration.secs);
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
    if(mplayer->current_music == NULL) {
        return;
    }

    if(Mix_PlayingMusic()) {
        text_info_t music_name = {18, NULL, NULL, white, {20, 0, 0, 0}};
        music_name.text_canvas.y = prevbtn_canvas->y - 10;
        music_name.utext = mplayer->current_music->music_name;
        SDL_Texture* texture = mplayer_renderunicode_text(mplayer, mplayer->music_font, &music_name);
        // As long as the texture returned is not NULL then we copy the texture to the renderer
        if(texture) {
            SDL_RenderCopy(mplayer->renderer, texture, NULL, &music_name.text_canvas);
            SDL_DestroyTexture(texture); texture = NULL;
        }
    }
}

void mplayer_renderprogress_bar(mplayer_t* mplayer, SDL_Color bar_color, SDL_Color line_color, 
    double curr_durationsecs, double full_durationsecs) {
    double percentage = round((double)(curr_durationsecs) / (double)(full_durationsecs) * (double)100);
    int percentageof = (int)round(((double)percentage / (double)100 * (double)mplayer->progress_bar.w));

    mplayer->progress_count.x = mplayer->progress_bar.x;
    mplayer->progress_count.y = mplayer->progress_bar.y;
    
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(bar_color));
    SDL_RenderDrawRect(mplayer->renderer, &mplayer->progress_bar);
    SDL_RenderFillRect(mplayer->renderer, &mplayer->progress_bar);

    if(Mix_PlayingMusic()) {
        mplayer->progress_count.w = percentageof;
        mplayer->progress_count.h = mplayer->progress_bar.h;
        if(mplayer->progressbar_clicked || mplayer->progressbar_dragged) {
            // Determine what percentage of the widtth of the progress bar was clicked
            int seek_position = mplayer->mouse_x - mplayer->progress_bar.x;
            percentage = (double)seek_position / (double)mplayer->progress_bar.w * (double)100;
            mplayer->progress_count.w = seek_position;
            /* Calculate the new duration based on the portion of the progress bar that was clicked
               and then set the new music position
            */
            curr_durationsecs = round(((double)percentage / (double)100) * (double)full_durationsecs);
            Mix_SetMusicPosition(curr_durationsecs);
            mplayer->progressbar_clicked = false; mplayer->progressbar_dragged = false;
        }
        SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(line_color));
        SDL_RenderDrawRect(mplayer->renderer, &mplayer->progress_count);
        SDL_RenderFillRect(mplayer->renderer, &mplayer->progress_count);
    }
}

void mplayer_controlmusic_progression(mplayer_t* mplayer) {
    if(!mplayer->music_list) {
        return;
    }
    music_t** music_list = &mplayer->music_list;
    size_t music_count = mplayer->music_count;
    if(mplayer->musicsearchbar_data && mplayer->music_searchresult_count > 0) {
        music_list = &mplayer->music_searchresult;
        music_count = mplayer->music_searchresult_count;
    }
    switch(mplayer->repeat_id) {
        case MUSIC_REPEATALLBTN:
            if(mplayer->playid < mplayer->music_count && mplayer->current_music && !Mix_PlayingMusic()) {
                // play the next music whenever one is completed
                if(mplayer->musicsearchbar_data && mplayer->music_searchresult_count > 0) {
                    mplayer->searchid++;
                    mplayer->searchid %= music_count;
                    mplayer->playid = (*music_list)[mplayer->searchid].searchmusic_id;
                } else {
                    mplayer->playid++;
                    mplayer->playid %= mplayer->music_count;
                }
                if(mplayer->music_list) {
                    mplayer->current_music = &mplayer->music_list[mplayer->playid];
                }
                // check if playing the music was successful and set music playing status to true if sucessful
                if(!Mix_PlayMusic(mplayer->current_music->music, 1)) {
                    mplayer->music_list[mplayer->playid].music_playing = true;
                }
            }
            break;
        case MUSIC_REPEATONEBTN:
            // fix skipping bug for repeat one btn
            if(!Mix_PlayingMusic() && mplayer->current_music) {
                if(mplayer->music_playing && !mplayer->current_music->music) {
                    if(mplayer->musicsearchbar_data && mplayer->music_searchresult_count > 0) {
                        mplayer->searchid++;
                        mplayer->searchid %= music_count;
                        mplayer->playid = (*music_list)[mplayer->searchid].searchmusic_id;
                    } else {
                        mplayer->playid++;
                        mplayer->playid %= mplayer->music_count;
                    }
                    mplayer->current_music = &mplayer->music_list[mplayer->playid];
                }
                if(!Mix_PlayMusic(mplayer->current_music->music, 1)) {
                    mplayer->music_list[mplayer->playid].music_playing = true;
                }
            }
            break;
        case MUSIC_REPEATOFFBTN:
            if(mplayer->playid < mplayer->music_count && mplayer->current_music && !Mix_PlayingMusic()) {
                // play the next music whenever one is completed
                if(mplayer->musicsearchbar_data && mplayer->music_searchresult_count > 0) {
                    mplayer->searchid++;
                    mplayer->searchid %= music_count;
                    mplayer->playid = (*music_list)[mplayer->searchid].searchmusic_id;
                } else {
                    mplayer->playid++;
                    mplayer->playid %= mplayer->music_count;
                }
                mplayer->current_music = &mplayer->music_list[mplayer->playid];
                // check if playing the music was successful and set music playing status to true if sucessful
                if(!Mix_PlayMusic(mplayer->current_music->music, 1)) {
                    mplayer->music_list[mplayer->playid].music_playing = true;
                }
                if(mplayer->musicsearchbar_data && mplayer->searchid == 0) {
                    mplayer->music_playing = false;
                    Mix_PauseMusic();
                } else if(!mplayer->musicsearchbar_data && mplayer->playid == 0) {
                    // This prevents the music from playing automatically whenever we reach the end of the music list
                    Mix_PauseMusic();
                    mplayer->music_playing = false;
                }
            }
            break;
    }
}

void mplayer_displayprogression_control(mplayer_t* mplayer) {
    mtime_t curr_duration = {0}, full_duration = {0};
    double full_durationsecs = 0, curr_durationsecs = 0;
    int progress = 0;
    if(Mix_PlayingMusic() && mplayer->current_music) {
        curr_durationsecs = Mix_GetMusicPosition(mplayer->current_music->music);
        full_durationsecs = mplayer->current_music->music_durationsecs;
        full_duration = mplayer->current_music->music_duration;
        curr_duration = mplayer_music_gettime(curr_durationsecs);
        if(full_durationsecs > 0.0) {
            progress = (int)(curr_durationsecs / full_durationsecs * 100.0);
        }
    }
    if(Mix_PlayingMusic() && !Mix_PausedMusic() && mplayer->current_music) {
        mplayer->music_playing = true;
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
    mplayer_controlmusic_progression(mplayer);
    if(mplayer->menu_opt == MPLAYER_DEFAULT_MENU) {
        mplayer_displaymusic_status(mplayer, curr_duration, full_duration);
        SDL_Color progressbar_color = {0x00, 0x00, 0x00, 0x00}, progress_linecolor = {0xFF, 0xFF, 0x00, 0xFF};
        mplayer_renderprogress_bar(mplayer, progressbar_color, progress_linecolor, curr_durationsecs, full_durationsecs);
    }
}

void mplayer_renderscroll_bar(mplayer_t* mplayer, mplayer_scrollbar_t* scrollbar, size_t max_textures) {
    int padding_x = scrollbar->padding_x, padding_y = scrollbar->padding_y;
    SDL_Rect* scrollbar_rect = &scrollbar->rect, scroll_area = scrollbar->scroll_area;
    double scroll_startpos = (double)scrollbar->start_pos, scroll_finalpos = (double)scrollbar->final_pos;

    // Calculate the initial position of the scrollbar    
    scrollbar_rect->x = scroll_area.w - scrollbar_rect->w + padding_x,
    scrollbar_rect->y = scroll_area.y + -padding_y;
    scrollbar->displacement = (double)scrollbar_rect->y - (double)scroll_area.y;

    /* Calculate the percentage of the music we are at based on the music render position
       the music render position is incremented by 1 when scrolling down and decremented by 1when scrolling up
    */
    double percentage = (scroll_startpos / scroll_finalpos);
    if(scroll_finalpos - scroll_startpos == max_textures) {
        percentage = 1.0;
    }
    // find x percentage of the songs box height - scrollbar height - distance - 2
    double percentage_of = ((double)scroll_area.h - (double)scrollbar_rect->h + (double)padding_y -
                            scrollbar->displacement) * percentage;
    int additional_y = (int)percentage_of;
    if(additional_y > 0) {
        // increment the scrollbar y position to let it move whenever a scroll event happens
        scrollbar_rect->y += additional_y;
    }

    // Draw the scrollbar to the screen
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(white));
    SDL_RenderFillRect(mplayer->renderer, scrollbar_rect);
    SDL_RenderDrawRect(mplayer->renderer, scrollbar_rect);
}

void mplayer_rendersongs(mplayer_t* mplayer) {
    /*printf("In mplayer_rendersongs():\n");
    printf("mplayer->music_count: %zu, mplayer->music_maxrenderpos: %zu\n", mplayer->music_count, mplayer->music_maxrenderpos);*/
    int cursor = MPLAYER_CURSOR_DEFAULT;
    text_info_t utext = {14, NULL, NULL, white, {songs_box.x + 2, songs_box.y + 1}};
    int default_w = 0, default_h = 0;
    SDL_Rect outer_canvas = utext.text_canvas;
    music_t* music_list = mplayer->music_list;
    size_t music_count = mplayer->music_count, max_renderpos = mplayer->music_maxrenderpos,
           *music_renderpos = &mplayer->music_renderpos;
    bool mutex_locked = false, searching = false;
    if(mplayer->musicsearchbar_data) {
        music_list = mplayer->music_searchresult;
        music_count = mplayer->music_searchresult_count;
        music_renderpos = &mplayer->music_searchrenderpos;
        max_renderpos = mplayer->match_maxrenderpos;
    }
    // if the music_count is zero then we exit the render songs function
    if(!music_count) {
        return;
    }    
    if(!music_list) {
        return;
    }
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
    int def_outerheight = 22;
    if(songs_box.h < def_outerheight) {
        def_outerheight = songs_box.h;
    }
    SDL_Rect prev_canvas = {0};
    size_t max_textures = 0, music_rendercount = 0, *music_renderlist = NULL, max_rendercount = 0;
    
    float max_textures_precision = roundf((float)songs_box.h /
        ((float)music_list[0].text_info.text_canvas.h + (float)def_outerheight));
    // Ensure that the max_textures_precision is not negative to prevent crashing
    if(max_textures_precision > 0) {
        max_textures = (size_t)max_textures_precision;
        max_rendercount = max_textures;
        music_renderlist = calloc(max_rendercount+1, sizeof(size_t));
        prev_canvas = music_list[0].outer_canvas;
        prev_canvas.x = songs_box.x + 2, prev_canvas.y = songs_box.y + 1;
        music_list[0].outer_canvas.y = songs_box.y + 1;
    }
    /* Ensure that the renderlist is not NULL. this happens whenever the max textures or max_textures precision
       is less than equal to zero
    */
    if(!music_renderlist) {
        return;
    }
    //printf("music_count is %ld\n", music_count);
    for(size_t i=0;i<music_count;i++) {
        utext = music_list[i].text_info;
        outer_canvas = music_list[i].outer_canvas;
        default_h = utext.text_canvas.h, default_w = utext.text_canvas.w;
        //printf("i: %ld, music_name: %ls\n", i, music_list[i].music_name);
        if(music_list[i].search_match && !mplayer->musicsearchbar_data) {
            music_list[i].search_match = false;
            music_list[i].search_render = false;
            if(songs_box.h < utext.text_canvas.h + def_outerheight) {
                music_list[i].outer_canvas.h = songs_box.h;
            } else {
                music_list[i].outer_canvas.h = utext.text_canvas.h + def_outerheight;
            }
        }
        SDL_Rect temp_canvas = music_list[i].outer_canvas;
        if(def_outerheight < 22 && music_list[i].text_info.text_canvas.h + def_outerheight < temp_canvas.h) {
            music_list[i].outer_canvas.h = music_list[i].text_info.text_canvas.h + def_outerheight;
        }
        if(mplayer->musicsearchbar_data && !music_list[i].search_render && mplayer->music_searchresult) { continue; }
        else if(!music_list[i].render && !mplayer->musicsearchbar_data) { continue; }
        // set the calculated x, y position and other related info for the music text and outer canvas
        music_list[i].outer_canvas.x = prev_canvas.x,
        music_list[i].outer_canvas.y = prev_canvas.y;
        music_list[i].outer_canvas.w = WIDTH - scrollbar.w;
        music_list[i].text_info.text_canvas.y = outer_canvas.y +
            ((music_list[i].outer_canvas.h -
            utext.text_canvas.h) / 2);
        //printf("Okay i'm here\n");
        outer_canvas = music_list[i].outer_canvas;
        utext = music_list[i].text_info;
        if(music_rendercount <= max_rendercount) {
            music_renderlist[music_rendercount] = i;
            // Handling scroll events
            if(mplayer->scroll && (mplayer->music_searchresult || !mplayer->musicsearchbar_data)) {
                size_t index = music_renderlist[music_rendercount];
                bool *music_renderstatus = (mplayer->music_searchresult) ? &music_list[i].search_render :
                                        &music_list[i].render;
                switch(mplayer->scroll_type) {
                    case MPLAYERSCROLL_DOWN:
                        int musicendpos_y = (music_list[max_renderpos].outer_canvas.y +
                        music_list[max_renderpos].text_info.text_canvas.h + def_outerheight);
                        if(max_renderpos - index >= max_textures && index < max_renderpos) {
                            music_list[index].outer_canvas.h -= 8;
                            if(music_list[index].outer_canvas.h <= (def_outerheight/2)) {
                                music_list[index].outer_canvas.h = 0;
                                *music_renderstatus = false;
                                (*music_renderpos)++;
                            }
                        } else if(max_renderpos - index >= max_textures && music_list[index].render
                            && music_list[index].outer_canvas.h <= (def_outerheight/2) && index < max_renderpos) {
                            // if the scroll up event occured before and the height of the outercanvas is zero
                            // then we set that particular music render status to be false
                            *music_renderstatus = false;
                            (*music_renderpos)++;
                        } else if(max_renderpos - index < max_textures && music_count >= max_textures
                            && musicendpos_y >= songs_box.y + songs_box.h && index < max_renderpos) {
                            // whenever we are close to the last texture in which the max_renderpos - index is less
                            // than the maximum amount of textures then we decrement the height of the texture at the
                            // top until it is less than or equal to zero
                            //printf("Decrementation 2\n");
                            music_list[index].outer_canvas.h -= 8;
                            if(music_list[index].outer_canvas.h <= (def_outerheight/2)) {
                                music_list[index].outer_canvas.h = 0;
                                *music_renderstatus = false;
                                (*music_renderpos)++;
                            }
                        }
                        break;
                    case MPLAYERSCROLL_UP:
                        if(index >= 0) {
                            music_list[index].outer_canvas.h += 8;
                            if(music_list[index].outer_canvas.h >= def_outerheight) {
                                music_list[index].outer_canvas.h = default_h + def_outerheight;
                                if(index > 0) {
                                    index--; (*music_renderpos)--;
                                    music_renderstatus = (mplayer->music_searchresult) ? &music_list[index].search_render :
                                        &music_list[index].render;     
                                }
                            }
                            *music_renderstatus = true;
                        }
                        break;
                }
                mplayer->scroll = false;
            }
            size_t next_index = (i < max_renderpos - 1) ? i+1 : i;
            SDL_Rect next_outercanvas = music_list[next_index].outer_canvas;
            SDL_Rect next_textcanvas = music_list[next_index].text_info.text_canvas;
            int musicendpos_y = (outer_canvas.y + utext.text_canvas.h + def_outerheight),
                nextendpos_y = next_outercanvas.y + next_textcanvas.h + def_outerheight;
            // calculate the amount we should decrease the original height of the outercanvas by to let it fit within
            // the songs box extra_h
            int extra_h = musicendpos_y - songs_box.y - songs_box.h + 5;
            // Whenever the current music position i is less than the maximum render position and the last music position
            // at the end is greater than the height of the songs box then we adjust the height to let it fit directly
            // within the songs box
            if(i <= max_renderpos && musicendpos_y > songs_box.y + songs_box.h) {
                if(music_rendercount == max_rendercount) {
                    extra_h -= 4;
                }
                if(i > 0) {
                    music_list[i].outer_canvas.h = utext.text_canvas.h + def_outerheight - extra_h;
                }
                music_list[i].fit = false;
                //music_list[i].fit = false;
            } else if(!music_list[i].fit && musicendpos_y < songs_box.y + songs_box.h &&
                def_outerheight == 22) {
                if(music_list[i].outer_canvas.y + utext.text_canvas.h + 22 < songs_box.y + songs_box.h) {
                        music_list[i].outer_canvas.h = utext.text_canvas.h + def_outerheight;
                }
                music_list[i].fit = true;
            }
            if(mplayer->music_searchresult || !mplayer->musicsearchbar_data) {
                SDL_SetRenderDrawColor(mplayer->renderer, 0x3B, 0x35, 0x61, 0xff);
                SDL_RenderDrawRect(mplayer->renderer, &music_list[music_renderlist[music_rendercount]].outer_canvas);
                SDL_RenderFillRect(mplayer->renderer, &music_list[music_renderlist[music_rendercount]].outer_canvas);
            }
            SDL_Color box_color = {0xff, 0xff, 0xff, 0xff}, tick_color = {0x00, 0xff, 0x00, 0xff},
            fill_color = {0xFF, 0xA5, 0x00, 0xff};
            checkbox_size.x = outer_canvas.x+5;
            checkbox_size.h = outer_canvas.h-10;
            checkbox_size.y = outer_canvas.y + ((outer_canvas.h - checkbox_size.h)/2);
            music_listplaybtn.btn_canvas.w = 30, music_listplaybtn.btn_canvas.h = outer_canvas.h - 10;
            music_listplaybtn.btn_canvas.x = (checkbox_size.x + checkbox_size.w) + 20,
            music_listplaybtn.btn_canvas.y = checkbox_size.y;
            int mouse_x = mplayer->mouse_x, mouse_y = mplayer->mouse_y;
            SDL_Rect hoverbg_canvas = {0};
            if(mplayer->music_searchresult || !mplayer->musicsearchbar_data) {
                // check if we hover over the play button and we haven't ticked any checkbox
                if(mplayer_musiclist_playbutton_hover(mplayer) && !mplayer->tick_count) {
                    hoverbg_canvas = music_listplaybtn.btn_canvas;
                    hoverbg_canvas.x -= 5, hoverbg_canvas.w += 5;
                    hoverbg_canvas.h = outer_canvas.h, hoverbg_canvas.y = outer_canvas.y;
                    SDL_SetRenderDrawColor(mplayer->renderer, 0x00, 0x00, 0x00, 0x00);
                    SDL_RenderDrawRect(mplayer->renderer, &hoverbg_canvas);
                    SDL_RenderFillRect(mplayer->renderer, &hoverbg_canvas);
                }
            }
            if(music_btns[MUSIC_PREVBTN].clicked && mplayer->playid >= 0) {
                // This prevents a music from going to the previous music when none was playing before
                if(Mix_PlayingMusic()) {
                    if(mplayer->musicsearchbar_data && mplayer->music_searchresult_count > 0) {
                        if(mplayer->searchid > 0) {
                            mplayer->searchid--;
                        }
                        mplayer->playid = music_list[mplayer->searchid].searchmusic_id;
                    } else {
                        if(mplayer->playid) {
                            mplayer->playid--;
                        }
                    }
                    mplayer->current_music = &mplayer->music_list[mplayer->playid];
                    Mix_HaltMusic();
                    if(!Mix_PausedMusic()) {
                        if(!Mix_PlayMusic(mplayer->current_music->music, 1)) {
                            mplayer->music_list[mplayer->playid].music_playing = true;
                        }
                    } else {
                        if(!Mix_PlayMusic(mplayer->current_music->music, 1)) {
                            mplayer->music_list[mplayer->playid].music_playing = true;
                        }
                        Mix_PauseMusic();
                    }
                }
                music_btns[MUSIC_PREVBTN].clicked = false;
            } else if(music_btns[MUSIC_SKIPBTN].clicked && mplayer->playid <= mplayer->music_count) {
                // This prevents a music from skipping to another when none was being played before
                if(Mix_PlayingMusic()) {
                    size_t prevplay_id = mplayer->playid;
                    // set the previous music playing status equal to false
                    if(mplayer->musicsearchbar_data && mplayer->music_searchresult_count > 0) {
                        mplayer->playid = music_list[mplayer->searchid].searchmusic_id;
                        // If a music was playing that was not in the search result then we set the playid equal to
                        // the first searchmusic_id in the search results list
                        if(mplayer->searchid == 0 && !mplayer->music_list[mplayer->playid].music_playing) {
                            mplayer->playid = music_list[mplayer->searchid].searchmusic_id;
                        } else {
                            mplayer->searchid++;
                        }
                        mplayer->searchid %= music_count;
                        mplayer->playid = music_list[mplayer->searchid].searchmusic_id;
                    } else {
                        mplayer->playid++;
                        mplayer->playid %= music_count;
                    }
                    if(prevplay_id != mplayer->playid) {
                        mplayer->music_list[prevplay_id].music_playing = false;
                    }
                    mplayer->current_music = &mplayer->music_list[mplayer->playid];
                    Mix_HaltMusic();
                    if(!Mix_PausedMusic()) {
                        if(!Mix_PlayMusic(mplayer->current_music->music, 1)) {
                            mplayer->music_list[mplayer->playid].music_playing = true;
                        }
                    } else {
                        if(!Mix_PlayMusic(mplayer->current_music->music, 1)) {
                            mplayer->music_list[mplayer->playid].music_playing = true;
                            Mix_PauseMusic();
                        }
                    }
                }
                music_btns[MUSIC_SKIPBTN].clicked = false;
            } else if(mplayer_music_hover(mplayer, i) && (mplayer->music_searchresult || !mplayer->musicsearchbar_data)) {
                // check if the mouse is hovered over the music
                if(mplayer_checkbox_hovered(mplayer)) {
                    if(mplayer->mouse_clicked) {
                        // if we are in the position of the checkbox and we clicked it
                        switch(music_list[i].checkbox_ticked) {
                            case false:
                                /* whenever the checkbox isn't already ticked then we set checkbox as ticked
                                   and the fill color for it as true
                                */
                                music_list[i].fill = true;
                                music_list[i].checkbox_ticked = true;
                                mplayer->tick_count++;
                                break;
                            case true:
                                /* whenever the checkbox is already ticked then we set the chekcbox as not ticked
                                   and the fill equal to false
                                */
                                music_list[i].fill = false;
                                music_list[i].checkbox_ticked = false;
                                mplayer->tick_count--;
                                break;
                        }
                        mplayer->mouse_clicked = false;
                    } else {
                        /* if we just hover over the checkbox without clicking it then we set that checkbox
                           fill equal to true
                        */
                        music_list[i].fill = true;
                    }
                    cursor = MPLAYER_CURSOR_POINTER;
                    mplayer_setcursor(mplayer, cursor);
                } else if(mplayer_musiclist_playbutton_hover(mplayer) && !mplayer->tick_count) {
                    if(mplayer->mouse_clicked) {
                        /* whenever we hover over the playbutton on the music
                           we determine if we should restart the current playing music or play a new music
                        */
                        if(Mix_PlayingMusic() && mplayer->playid == i) {
                            Mix_SetMusicPosition(0);
                            if(Mix_PausedMusic()) {
                                Mix_ResumeMusic();
                                music_list[i].music_playing = true;
                            }
                        } else {
                            mplayer->current_music = &music_list[i];
                            if(Mix_PlayingMusic()) {
                                Mix_HaltMusic();
                            }
                            mplayer->playid = i;
                            if(mplayer->musicsearchbar_data) {
                                mplayer->current_music = &mplayer->music_list[music_list[i].searchmusic_id];
                                mplayer->playid = music_list[i].searchmusic_id;
                                mplayer->searchid = i;
                            }
                            music_list[i].music_playing = true;
                            if(Mix_PlayMusic(mplayer->current_music->music, 1) == -1) {
                                music_list[i].music_playing = false;
                                printf("THe music failed %ls to play\n", music_list[i].music_name);
                            }
                        }
                        mplayer->mouse_clicked = false;
                    }
                    cursor = MPLAYER_CURSOR_POINTER;
                    mplayer_setcursor(mplayer, cursor);
                } else if(mplayer->mouse_clicked) {
                    // whenever we click the music without clicking any of its elements
                    // we set clicked equal to false to prevent it from performing any action that we do not want
                    if(music_list[i].checkbox_ticked) {
                        mplayer->tick_count--;
                        music_list[i].fill = false;
                        music_list[i].checkbox_ticked = false;
                    } else if(mplayer->tick_count) {
                        mplayer->tick_count++;
                        music_list[i].fill = true;
                        music_list[i].checkbox_ticked = true;
                    }
                    mplayer->mouse_clicked = false;
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
            if(mplayer->music_searchresult || !mplayer->musicsearchbar_data) {
                if(mplayer->tick_count) {
                    // whenever any checkbox is ticked we determine whether to render the current music as ticked or not
                    if(!music_list[i].checkbox_ticked && !mplayer_music_hover(mplayer, i)) {
                        music_list[i].fill = false;
                    }
                    mplayer_drawmusic_checkbox(mplayer, box_color, fill_color, music_list[i].fill, tick_color,
                    music_list[i].checkbox_ticked);
                } else if(mplayer_music_hover(mplayer, i)) {
                    // whenever no checkbox is ticked and we hover over the music we display the check box
                    // so that the user can click it or select it
                    if(mplayer_checkbox_hovered(mplayer)) {
                        mplayer_drawmusic_checkbox(mplayer, box_color, fill_color, music_list[i].fill, tick_color,
                        music_list[i].checkbox_ticked);
                    } else if(!mplayer_checkbox_hovered(mplayer)) {
                        mplayer_drawmusic_checkbox(mplayer, box_color, fill_color, false, tick_color, false);
                    }
                }
                SDL_RenderCopy(mplayer->renderer, music_list[music_renderlist[music_rendercount]].text_texture, NULL,
                    &utext.text_canvas);
                music_rendercount++;
            }
        } else {
            break;
        }
        // calculate the y position for the next musics outercanvas
        prev_canvas.y += music_list[i].outer_canvas.h + 3;
    }
    /* Determine the index, and Calculate the end y position of the last texture that was rendered to the screen */
    size_t index = (music_rendercount < 1) ? 0 : music_renderlist[music_rendercount-1];
    int endpos_y = music_list[index].outer_canvas.y + music_list[index].text_info.text_canvas.h + def_outerheight;
    /* Calculate the end y position for the songs_box and Initialize songs_renderheight_total to store the total height
       of the textures that are currently being rendered to the screen */
    int songsbox_endpos_y = songs_box.y + songs_box.h, songs_renderheight_total = 0;

    /* The data and calculations above are used whenever the window is resized and textures cannot fit on the screen.
       These conditions are evaluated:
       1. the index of the last music that was rendered to the screen is equal to the maximum render position
       2. The end y position of the songs box - the end y position for the last music that is being rendered is greater
       than the height of the text for the last texture
       As long as the conditions above are true then we adjust the height of the previous textures that are not being
       rendered to the screen so that they fit entirely within the songs_box
    */
    if(mplayer->window_resized && index == max_renderpos && songsbox_endpos_y - endpos_y > music_list[index].text_info.text_canvas.h) {
        /* Determine the total height of each texture that is currently being rendered to the screen */
        for(size_t i=0;i<music_rendercount;i++) {
            songs_renderheight_total += music_list[music_renderlist[i]].outer_canvas.h;
        }
        /* Using the total height of each texture being rendered. We determine the amount of textures that are
           not being rendered to the screen by finding the difference in the height of the songs box and the
           total height of the textures being rendered to the screen
        */
        int amount_notbeing_rendered = (int)roundf((float)(songs_box.h - songs_renderheight_total) /
        (float)(music_list[0].text_info.text_canvas.h + def_outerheight));
        int amount_being_rendered = (max_textures-1) - amount_notbeing_rendered;
        /*printf("songs_renderheight_total = %d, amount_notbeingrendered = %d, amount_beingrendered: %d\n",
        songs_renderheight_total, amount_notbeing_rendered, amount_being_rendered);
        songs_box.h - songs_renderheight_total;*/
        
        /* Whenever the first render position is greater than the amount of textures not being rendered then
           we adjust the height of previous render positions and the first_render position
           so that it fits within the screen.
        */
        size_t first_renderpos = music_renderlist[0];
        if(first_renderpos > amount_notbeing_rendered) {
            for(size_t i=first_renderpos-amount_notbeing_rendered;i<=first_renderpos;i++) {
                bool* render_status = (mplayer->music_searchresult) ? &music_list[i].search_render :
                                        &music_list[i].render;
                music_list[i].outer_canvas.h = music_list[i].text_info.text_canvas.h + def_outerheight;
                //music_list[i].render = true;
                mplayer->music_renderpos--;
                *render_status = true;
            }
        }
    }
    // scroll bar related information
    mplayer_scrollbar_t songsbox_scrollbar = {
        .rect = scrollbar,
        .displacement = 0.0,
        .orientation = 0,
        .start_pos = *music_renderpos,
        .final_pos = music_count,
        .padding_x = -2,
        .padding_y = -(2 + (int)((double)scrollbar.h / 2.0)),
        .scroll_area = songs_box
    };
    mplayer_renderscroll_bar(mplayer, &songsbox_scrollbar, max_textures);
    mplayer->window_resized = false;
    free(music_renderlist); music_renderlist = NULL;
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
            menu->textures[MPLAYER_BUTTON_TEXTURE][i] = IMG_LoadTexture(mplayer->renderer, music_btns[i].imgbtn_path);
            menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][i] = music_btns[i].btn_canvas;
            music_btns[i].texture_idx = i;
        }
        
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

        // Add music to playlist button
        mplayer_addmenu_texture(mplayer, MPLAYER_BUTTON_TEXTURE);
        music_addplaylistbtn.texture_idx = menu->texture_sizes[MPLAYER_BUTTON_TEXTURE]-1;
        menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][music_addplaylistbtn.texture_idx] = music_addplaylistbtn.btn_canvas;
    } else if(mplayer->menu_opt == MPLAYER_SETTINGS_MENU && menu->textures[MPLAYER_TEXT_TEXTURE] == NULL) {
        mplayer_createmenu_texture(mplayer, MPLAYER_TEXT_TEXTURE, setting_textinfo_size);
        mplayer_createmenu_texture(mplayer, MPLAYER_BUTTON_TEXTURE, SETTINGSBTN_COUNT);
        for(size_t i=0;i<SETTINGSBTN_COUNT;i++) {
            menu->textures[MPLAYER_BUTTON_TEXTURE][i] = IMG_LoadTexture(mplayer->renderer, setting_btns[i].imgbtn_path);
            menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][i] = setting_btns[i].btn_canvas;
            setting_btns[i].texture_idx = i;
        }

        // music location remove button
        mplayer_addmenu_texture(mplayer, MPLAYER_BUTTON_TEXTURE);
        music_removebtn.texture_idx = menu->texture_sizes[MPLAYER_BUTTON_TEXTURE]-1;
        menu->textures[MPLAYER_BUTTON_TEXTURE][music_removebtn.texture_idx] = IMG_LoadTexture(mplayer->renderer,
            music_removebtn.imgbtn_path);
        menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][music_removebtn.texture_idx] = music_removebtn.btn_canvas;
    }
}

void mplayer_run(mplayer_t* mplayer) {
    //pthread_t search_thread = 0, searchbar_thread = 0;
    //pthread_create(&search_thread, NULL, mplayer_searchthread, mplayer);
    mplayer_setup_menu(mplayer);
    while(!mplayer->quit) {
        if(mplayer->menu_opt == MPLAYER_DEFAULT_MENU) {
            mplayer_defaultmenu(mplayer);
        } else if(mplayer->menu_opt == MPLAYER_SETTINGS_MENU) {
            mplayer_settingmenu(mplayer);
        }
    }
    //pthread_join(search_thread, NULL);
}

void mplayer_defaultmenu(mplayer_t* mplayer) {
    int tab_hoverid = 0;
    int music_id = 0;
    //SDL_GetTicks();
    mplayer_set_window_title(mplayer, WINDOW_TITLE);
    while(SDL_PollEvent(&mplayer->e)) {
        if(mplayer->e.type == SDL_QUIT) {
            mplayer->quit = 1; break;
        } else if(mplayer->e.type == SDL_WINDOWEVENT && mplayer->e.window.event == SDL_WINDOWEVENT_RESIZED) {
            mplayer->window_resized = true;
        }  else if(mplayer->e.type == SDL_TEXTINPUT) {
            if(mplayer->musicsearchbar_clicked) {
                if(mplayer->searchthread_created) {
                    pthread_join(mplayer->search_thread, NULL);
                    mplayer->searchthread_created = false;
                } else if(!mplayer->searchthread_creationtimer) {
                    mplayer->searchthread_creationtimer = SDL_GetTicks() + 500;
                }
                mplayer->music_searchresult_ready = false;
                char* temp_text = NULL, *temp2_text = NULL;
                size_t text_len = strlen(mplayer->e.text.text), text2_len = 0;
                size_t music_searchbuffer_len = mplayer->musicsearchbar_datalen + text_len;
                // use malloc instead of calloc because it is faster
                char* music_searchbuffer = calloc(music_searchbuffer_len+1, sizeof(char));
                if(mplayer->musicsearchbar_data) {
                    // Get the text before the cursor position
                    if(mplayer->musicsearchcursor_relpos > 0 ||
                        mplayer->musicsearchcursor_relpos < mplayer->musicsearchbar_datalen) {
                        temp_text = malloc(mplayer->musicsearchcursor_relpos * sizeof(char));
                    }
                    // when text is found before the cursor position we store it in temp_text
                    if(temp_text) {
                        strncpy(temp_text, mplayer->musicsearchbar_data, mplayer->musicsearchcursor_relpos);
                        mplayer->musicsearchbar_data += mplayer->musicsearchcursor_relpos;
                        /* Obtain the length of the data after the cursor position if it is greater than zero
                           then we store the text that is found after the cursor position
                        */
                        text2_len = strlen(mplayer->musicsearchbar_data);
                        if(text2_len > 0) {
                            temp2_text = malloc(text2_len * sizeof(char));
                            strncpy(temp2_text, mplayer->musicsearchbar_data, text2_len);
                            //printf("%s\n", temp2_text);
                        }
                        mplayer->musicsearchbar_data -= mplayer->musicsearchcursor_relpos;
                    }
                }
                // whenever we find text before the cursor position we store it into the search data variable
                if(temp_text) {
                    strncpy(music_searchbuffer, temp_text, mplayer->musicsearchcursor_relpos);
                }
                // Insert the particular character that the user types in at a particular position
                strncat(music_searchbuffer, mplayer->e.text.text, text_len);
                // then copy the text that was after the cursor position
                if(temp2_text) {
                    strncat(music_searchbuffer, temp2_text, text2_len);
                }
                mplayer->musicsearchcursor_relpos++;
                mplayer->musicsearchbar_datalen = music_searchbuffer_len;
                mplayer->musicsearchbar_data = music_searchbuffer;
                mplayer->music_newsearch = true;
                mplayer->start_search = true;
                free(temp_text); free(temp2_text);
                temp_text = NULL; temp2_text = NULL;
            }
        } else if(mplayer->e.type == SDL_KEYDOWN) {
            switch(mplayer->e.key.keysym.scancode) {
                case SDL_SCANCODE_BACKSPACE:
                    if(mplayer->musicsearchbar_clicked && mplayer->musicsearchbar_datalen > 0) {
                        if(mplayer->musicsearchcursor_relpos > 0) {
                            mplayer->musicsearchbar_datalen--;
                            mplayer->musicsearchcursor_relpos--;
                            if(mplayer->musicsearchbar_datarenderpos > 0) {
                                mplayer->musicsearchbar_datarenderpos--;
                            }
                            mplayer->musicsearchbar_data[mplayer->musicsearchcursor_relpos] = 0;
                            for(size_t i=mplayer->musicsearchcursor_relpos;i<mplayer->musicsearchbar_datalen;i++) {
                                mplayer->musicsearchbar_data[i] = mplayer->musicsearchbar_data[i+1];
                            }
                            mplayer->musicsearchbar_data[mplayer->musicsearchbar_datalen] = 0;
                        }
                        if(!mplayer->musicsearchbar_datalen) {
                            free(mplayer->musicsearchbar_data); mplayer->musicsearchbar_data = NULL;
                            mplayer->music_newsearch = false;
                        } else {
                            char* newsearchbar_data = calloc(mplayer->musicsearchbar_datalen+1, sizeof(char));
                            newsearchbar_data[mplayer->musicsearchbar_datalen] = 0;
                            strcpy(newsearchbar_data, mplayer->musicsearchbar_data);
                            free(mplayer->musicsearchbar_data); mplayer->musicsearchbar_data = newsearchbar_data;
                            if(mplayer->searchthread_created) {
                                pthread_join(mplayer->search_thread, NULL);
                                mplayer->searchthread_created = false;
                            } else if(!mplayer->searchthread_creationtimer) {
                                mplayer->searchthread_creationtimer = SDL_GetTicks() + 1000;
                            }
                            mplayer->music_searchresult_ready = false;
                            mplayer->music_newsearch = true;
                        }
                        mplayer->start_search = true;
                    }
                    break;
                case SDL_SCANCODE_RIGHT:
                    if(mplayer->musicsearchcursor_relpos < (int)mplayer->musicsearchbar_datalen) {
                        mplayer->musicsearchcursor_relpos++;
                        int w = 0, h = 0;
                        TTF_SizeText(mplayer->music_font, mplayer->musicsearchbar_data, &w, NULL);
                        if(w > mplayer->music_searchbar.w) {
                            mplayer->musicsearchbar_datarenderpos++;
                        }
                    }
                    break;
                case SDL_SCANCODE_LEFT:
                    if(mplayer->musicsearchcursor_relpos >= 1) {
                        mplayer->musicsearchcursor_relpos--;
                        int w = 0, h = 0;
                        TTF_SizeText(mplayer->music_font, mplayer->musicsearchbar_data, &w, &h);
                        if(mplayer->musicsearchbar_datarenderpos > 0) {
                            mplayer->musicsearchbar_datarenderpos--;
                        }
                    }
                    break;
            }
        } else if(mplayer->e.type == SDL_MOUSEMOTION) {
            mplayer->mouse_x = mplayer->e.motion.x, mplayer->mouse_y = mplayer->e.motion.y;
            if(mplayer_ibutton_hover(mplayer, setting_iconbtn)) {
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
                setting_iconbtn.hover = true;
            } else if(mplayer_ibutton_hover(mplayer, music_addfolderbtn)) {
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
                music_addfolderbtn.hover = true;
            } else if(mplayer_tabs_hover(mplayer, tab_info, &tab_hoverid, tab_info_size) && TAB_INIT) {
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
            } else if(mplayer_progressbar_hover(mplayer) && Mix_PlayingMusic()) {
                if(mplayer->mouse_buttondown) {
                    mplayer->progressbar_dragged = true;
                }
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
            } else if(mplayer_searchbar_hover(mplayer)) {
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_TYPEABLE);
            } else {
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_DEFAULT);
                setting_iconbtn.hover = false;
            }
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
        } else if(mplayer->e.type == SDL_MOUSEBUTTONDOWN) {
            mplayer->mouse_x = mplayer->e.button.x, mplayer->mouse_y = mplayer->e.button.y;
            mplayer->mouse_buttondown = true;
        } else if(mplayer->e.type == SDL_MOUSEBUTTONUP) {
            mplayer->mouse_buttondown = false;
            mplayer->mouse_x = mplayer->e.button.x, mplayer->mouse_y = mplayer->e.button.y;
            if(mplayer_tabs_hover(mplayer, tab_info, &tab_hoverid, tab_info_size) && TAB_INIT) {
                tab_info[prev_tab].active = false;
                tab_info[tab_hoverid].underline_color = underline_color;
                tab_info[tab_hoverid].active = true;
                active_tab = tab_hoverid;
            } else if(mplayer_progressbar_hover(mplayer) && Mix_PlayingMusic()) {
                mplayer->progressbar_clicked = true;
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
            mplayer->mouse_clicked = true;
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
    mplayer_tooltip_t add_folder_tooltip = {
        .background_color = {0x22, 0x18, 0x1C, 0xFF},
        .text_color = {0xC5, 0xFF, 0xFD, 0xFF},
        .text = music_addfolderbtn.tooltip_text,
        .margin_x = 10,
        .margin_y = 10,
        .x = -1,
        .y = 0,
        .element_canvas = music_addfolderbtn.btn_canvas,
        .delay_secs = 0,
        .duration_secs = 0,
        .font = mplayer->music_font,
        .font_size = 18
    };
    mplayer_rendertooltip(mplayer, &add_folder_tooltip);

    // Create settings button on screen
    setting_iconbtn.btn_canvas.x = WIDTH - setting_iconbtn.btn_canvas.w - 2;
    mplayer->menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][setting_iconbtn.texture_idx] =
        setting_iconbtn.btn_canvas;
    SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][setting_iconbtn.texture_idx],
        NULL, &setting_iconbtn.btn_canvas);
    mplayer_tooltip_t settings_button_tooltip = {
        .background_color = {0x22, 0x18, 0x1C, 0xFF},
        .text_color = {0xC5, 0xFF, 0xFD, 0xFF},
        .text = setting_iconbtn.tooltip_text,
        .margin_x = 10,
        .margin_y = 10,
        .x = -1,
        .y = 0,
        .element_canvas = setting_iconbtn.btn_canvas,
        .delay_secs = 0,
        .duration_secs = 0,
        .font = mplayer->music_font,
        .font_size = 18
    };
    mplayer_rendertooltip(mplayer, &settings_button_tooltip);
    if(!mplayer->music_list) {
        mplayer_loadmusics(mplayer);
        if(mplayer->musicsearchbar_data && mplayer->music_list) {
            mplayer->music_newsearch = true;
        }
    }
    /* Create music bar */
    mplayer_createmusicbar(mplayer);
    mplayer_displayprogression_control(mplayer);
    if(active_tab == SONGS_TAB) {
        if(mplayer->musicsearchbar_data && !mplayer->searchthread_created) {
            if(mplayer->update_searchresults || (SDL_TICKS_PASSED(SDL_GetTicks(), mplayer->searchthread_creationtimer)
                && mplayer->searchthread_creationtimer > 0)) {
                pthread_create(&mplayer->search_thread, NULL, &mplayer_searchthread, mplayer);
                mplayer->searchthread_created = true;
                mplayer->searchthread_creationtimer = 0;
            }
        }
        /* Create the search bar for searching for music */
        mplayer_createsearch_bar(mplayer);
        /* Create songs box */
        mplayer_createsongs_box(mplayer);
        if(mplayer->music_searchresult_ready && mplayer->searchthread_created) {
            pthread_join(mplayer->search_thread, NULL);
            mplayer->searchthread_created = false;
            mplayer->start_search = false;
        }
        mplayer_populate_searchresults(mplayer);
        // render songs so we can set the new music position based on the percentage of the music we are in
        // whether in the search result or the regular music list
        mplayer_rendersongs(mplayer);
    } else if(active_tab == ALBUMS_TAB) {
    } else if(active_tab == QUEUES_TAB) {
    }
    prev_tab = active_tab;

    // Present all rendered objects on the screen
    SDL_RenderPresent(mplayer->renderer);
    mplayer_destroytextures(mplayer->menu->textures[MPLAYER_TEXT_TEXTURE], text_info_size-1);
    mplayer_destroytextures(mplayer->menu->textures[MPLAYER_TAB_TEXTURE], tab_info_size);
    mplayer_menu_freetext(mplayer, MPLAYER_DEFAULT_MENU);
}

void mplayer_settingmenu(mplayer_t* mplayer) {
    text_info_t music_location = {24, NULL, NULL, {0xE5, 0x58, 0x12, 0xFF}/*white*/, {0}};
    SDL_Rect* canvas = NULL;
    int btn_id = 0;
    bool mouse_clicked = false;
    mplayer_set_window_title(mplayer, SETTING_TITLE);
    mplayer_set_window_color(mplayer->renderer, setting_wincolor);
    while(SDL_PollEvent(&mplayer->e)) {
        if(mplayer->e.type == SDL_QUIT) {
            mplayer->quit = 1;
            break;
        } else if(mplayer->e.type == SDL_MOUSEMOTION) {
            mplayer->mouse_x = mplayer->e.motion.x, mplayer->mouse_y = mplayer->e.motion.y;
            if(mplayer_ibuttons_hover(mplayer, setting_btns, &btn_id, SETTINGSBTN_COUNT)) {
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
            } else {
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_DEFAULT);
            }
        } else if(mplayer->e.type == SDL_MOUSEBUTTONUP) {
            mplayer->mouse_x = mplayer->e.button.x, mplayer->mouse_y = mplayer->e.button.y;
            if(mplayer_ibuttons_hover(mplayer, setting_btns, &btn_id, SETTINGSBTN_COUNT)) {
                if(btn_id == BACK_BUTTON) {
                    mplayer->menu_opt = MPLAYER_DEFAULT_MENU;
                    mplayer_setup_menu(mplayer);
                    mplayer_setcursor(mplayer, MPLAYER_CURSOR_DEFAULT);
                    return;
                }
            }
            mouse_clicked = true;
        }
    }
    SDL_GetWindowSize(mplayer->window, &WIDTH, &HEIGHT);
    mplayer_displayprogression_control(mplayer);
    // create buttons text, normal and display on the screen
    for(size_t i=0;i<setting_textinfo_size;i++) {
        mplayer->menu->textures[MPLAYER_TEXT_TEXTURE][i] = mplayer_rendertext(mplayer, mplayer->font, &setting_textinfo[i]);
    }

    // set the canvas x position of the text "Go Back To Home" beside the back button 
    canvas = &setting_textinfo[0].text_canvas;
    canvas->x = setting_btns[0].btn_canvas.x + setting_btns[0].btn_canvas.w + 5;

    SDL_Rect bg_canvas = {0};
    SDL_Color bg_canvascolor = (SDL_Color){0x39, 0x37, 0x5B, 0xFF}/*{0x4B, 0x3B, 0x40, 0xFF}{0x0F, 0x52, 0x57, 0xFF}{0x81, 0x17, 0x1B, 0xFF}{0x20, 0x81, 0xC3, 0xFF}{0x28, 0x30, 0x44, 0xFF}*/;
    bg_canvas.w = WIDTH, bg_canvas.h = setting_btns[0].btn_canvas.y + setting_btns[0].btn_canvas.h + SETTING_LINESPACING;
    
    mplayer_tooltip_t backbtn_tooltip = {
        .background_color = {0x09, 0x0C, 0x9B, 0xFF},
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
        .font_size = 18
    };
    
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(bg_canvascolor));
    SDL_RenderDrawRect(mplayer->renderer, &bg_canvas);
    SDL_RenderFillRect(mplayer->renderer, &bg_canvas);

    canvas->y = roundf((float)(bg_canvas.h - canvas->h) / (float)2);
    setting_btns[0].btn_canvas.y = roundf((float)(bg_canvas.h - setting_btns[0].btn_canvas.h) / (float)2);
    for(size_t i=1;i<setting_textinfo_size;i++) {
        setting_textinfo[i].text_canvas.y = setting_textinfo[i-1].text_canvas.y + setting_textinfo[i-1].text_canvas.h + SETTING_LINESPACING;
    }
    // Draw a background for the Music Location category
    canvas = &setting_textinfo[1].text_canvas;
    bg_canvascolor = (SDL_Color){0x0F, 0x52, 0x57, 0xFF};
    bg_canvas = (SDL_Rect){0};
    bg_canvas.x = 0, bg_canvas.y = canvas->y;
    bg_canvas.w = WIDTH, bg_canvas.h = canvas->h + SETTING_LINESPACING;
    canvas->x = 20/*roundf((float)(WIDTH - canvas->w)/(float)2)*/;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(bg_canvascolor));
    SDL_RenderDrawRect(mplayer->renderer, &bg_canvas);
    SDL_RenderFillRect(mplayer->renderer, &bg_canvas);
    
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
    bg_canvas.y = setting_textinfo[1].text_canvas.y + roundf((float)(bg_canvas.h - (canvas->h - SETTING_LINESPACING))
        / (float)2);

    if(mplayer_rect_hover(mplayer, bg_canvas)) {
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
        if(mouse_clicked) {
            mplayer_browsefolder(mplayer);
        }
    }

    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(bg_canvascolor));
    SDL_RenderDrawRect(mplayer->renderer, &bg_canvas);
    SDL_RenderFillRect(mplayer->renderer, &bg_canvas);

    // copy each texture on its particular canvas
    for(size_t i=0;i<setting_textinfo_size;i++) {
        mplayer->menu->texture_canvases[MPLAYER_TEXT_TEXTURE][i] = setting_textinfo[i].text_canvas;
        SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_TEXT_TEXTURE][i], NULL,
            &mplayer->menu->texture_canvases[MPLAYER_TEXT_TEXTURE][i]);
    }

    // Render each music location from the music paths info file
    music_location.text_canvas.x = 50;
    music_location.text_canvas.y = canvas->y + canvas->h + SETTING_LINESPACING;
    bg_canvas.x = 0, bg_canvas.y = music_location.text_canvas.y;
    bg_canvascolor = (SDL_Color){0x4B, 0x3B, 0x40, 0xFF}
    /*(SDL_Color){0x0F, 0x52, 0x57, 0xFF}(SDL_Color){0x81, 0x17, 0x1B, 0xFF}(SDL_Color){0x1E, 0x96, 0xFC, 0xFF}{0x58, 0x72, 0x91, 0xFF}*/;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(bg_canvascolor));
    for(size_t i=0;i<mplayer->location_count;i++) {
        music_location.utext = mplayer->locations[i].path;
        mplayer_get_textsize(mplayer->music_font, music_location.font_size, &music_location);
        bg_canvas.w = WIDTH, bg_canvas.h = music_location.text_canvas.h + SETTING_LINESPACING;
        music_location.text_canvas.y = bg_canvas.y + roundf((float)(bg_canvas.h - music_location.text_canvas.h) / (float)2);
    
        canvas = &music_removebtn.btn_canvas;
        canvas->x = WIDTH - (canvas->w * 2);
        canvas->y = music_location.text_canvas.y;
        if(mplayer_ibutton_hover(mplayer, music_removebtn)) {
            mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
            if(mouse_clicked) {
                printf("removing music location %ls along with related musics\n", mplayer->locations[i].path);
                mplayer_delmusic_locationindex(mplayer, i);;
                mouse_clicked = false;
                continue;
            }
        }
        SDL_Texture* texture = mplayer_renderunicode_text(mplayer, mplayer->music_font, &music_location);
        SDL_RenderDrawRect(mplayer->renderer, &bg_canvas);
        SDL_RenderFillRect(mplayer->renderer, &bg_canvas);
        mplayer_addmenu_texture(mplayer, MPLAYER_TEXT_TEXTURE);
        mplayer_menuplace_texture(mplayer, MPLAYER_TEXT_TEXTURE, texture, music_location.text_canvas);
        bg_canvas.y = bg_canvas.y + bg_canvas.h + SETTING_LINESPACING;
        SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][music_removebtn.texture_idx],
            NULL, canvas);
        SDL_RenderCopy(mplayer->renderer,
            mplayer->menu->textures[MPLAYER_TEXT_TEXTURE][mplayer->menu->texture_sizes[MPLAYER_TEXT_TEXTURE]-1], NULL,
            &mplayer->menu->texture_canvases[MPLAYER_TEXT_TEXTURE][mplayer->menu->texture_sizes[MPLAYER_TEXT_TEXTURE]
            - 1]);
    }

    // render buttons on screen
    for(size_t i=0;i<SETTINGSBTN_COUNT;i++) {
        mplayer->menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][i] = setting_btns[i].btn_canvas;
        SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][i], NULL,
            &setting_btns[i].btn_canvas);
    }
    mplayer_rendertooltip(mplayer, &backbtn_tooltip);
    SDL_RenderPresent(mplayer->renderer);
    mplayer_destroytextures(mplayer->menu->textures[MPLAYER_TEXT_TEXTURE],
        mplayer->menu->texture_sizes[MPLAYER_TEXT_TEXTURE]);
    mplayer_menu_freetext(mplayer, MPLAYER_SETTINGS_MENU);
}