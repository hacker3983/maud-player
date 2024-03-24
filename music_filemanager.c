#include "music_player.h"

#ifdef _WIN32
void mplayer_getroot_path(char* root_path) {
        size_t directory_length = GetCurrentDirectoryA(0, NULL);
        char* directory = calloc(directory_length, sizeof(char));
        GetCurrentDirectoryA(directory_length, directory);
        SetCurrentDirectoryA("/");
        GetCurrentDirectoryA(4, root_path);
        SetCurrentDirectoryA(directory);
        free(directory); directory = NULL;
}
#endif

void mplayer_getmusic_locations(mplayer_t* mplayer) {
    FILE* f = fopen(MUSIC_PATHINFO_FILE, "r");
    if(f == NULL) {
        printf("PATH DOES NOT EXIST\n");
        mplayer->music_count = 0;
        mplayer->music_list = NULL;
        mplayer->musinfo.files = NULL;
        mplayer->musinfo.locations = NULL;
        mplayer->musinfo.file_count = 0;
        mplayer->musinfo.location_count = 0;
        return;
    }
    musloc_t* music_loclist = calloc(1, sizeof(musloc_t));
    char c = '\0';
    #ifdef _WIN32
    wchar_t *music_loc = calloc(2, sizeof(wchar_t)), wc = L'\0';
    #else
    char *music_loc = calloc(2, sizeof(char));
    #endif
    size_t mloc_len = 0, muslist_count = 0;
    while((c = fgetc(f)) != EOF) {
        #ifdef _WIN32
        mbtowc(&wc, &c, 1);
        if(wc == L'\n') {
            music_loclist[muslist_count++].path = music_loc;
            music_loclist = realloc(music_loclist, (muslist_count + 1) * sizeof(musloc_t));
            music_loclist[muslist_count].path = NULL;
            music_loc = calloc(2, sizeof(wchar_t));
            mloc_len = 0;
            continue;
        }
        music_loc[mloc_len++] = wc;
        wchar_t* temp = music_loc;
        music_loc = calloc(mloc_len + 1, sizeof(wchar_t));
        wcsncpy(music_loc, temp, mloc_len);
        free(temp); temp = NULL;
        #else
        if(c == '\n') {
            music_loclist[muslist_count++].path = music_loc;
            music_loclist = realloc(music_loclist, (muslist_count + 1) * sizeof(musloc_t));
            music_loclist[muslist_count].path = NULL;
            music_loc = calloc(2, sizeof(char));
            mloc_len = 0;
            continue;
        }
        music_loc[mloc_len++] = c;
        char* temp = music_loc;
        music_loc = calloc(mloc_len + 1, sizeof(char));
        strcpy(music_loc, temp);
        free(temp); temp = NULL;
        #endif
    }
    (mloc_len == 0) ? free(music_loc) : (music_loclist[muslist_count++].path = music_loc);
    fclose(f);
    mplayer->musinfo.locations = music_loclist;
    mplayer->musinfo.location_count = muslist_count;
}

void mplayer_getmusic_filepaths(mplayer_t* mplayer) {
    if(mplayer->musinfo.locations == NULL) {
        mplayer->music_count = 0;
        return;
    }
    musloc_t* music_files = calloc(1, sizeof(musloc_t));
    size_t mfile_count = 0;

    for(size_t i=0;i<mplayer->musinfo.location_count;i++) {
        #ifdef _WIN32
        size_t location_len = wcslen(mplayer->musinfo.locations[i].path);
        #else
        size_t location_len = strlen(mplayer->musinfo.locations[i].path);
        #endif
        size_t pathpat_len = location_len + 8;
        #ifdef _WIN32
        wchar_t* path_pattern = calloc(pathpat_len, sizeof(wchar_t));
        for(int j=0;FILE_EXTENSIONS[j] != NULL;j++) {
            wchar_t *wstr = mplayer_stringtowide(FILE_EXTENSIONS[j]);
            wcscpy(path_pattern, mplayer->musinfo.locations[i].path);
            wcscat(path_pattern, L"\\*.");
            wcscat(path_pattern, wstr);
            free(wstr); wstr = NULL;

            WIN32_FIND_DATAW fd = {0};
            HANDLE hfind = FindFirstFileW(path_pattern, &fd);
            if(hfind == INVALID_HANDLE_VALUE) {
                printf("No file extension found\n");
                continue;
            }
            do {
                char *altstr = mplayer_widetostring(fd.cAlternateFileName),
                    *altpathstr = mplayer_widetostring(mplayer->musinfo.locations[i].path);
                size_t length_str = wcslen(fd.cFileName), length_altstr = wcslen(fd.cAlternateFileName);
                size_t path_len = location_len + length_str + 7,
                    altpath_len = location_len + length_altstr + 7;

                music_files[mfile_count].path = calloc(path_len+1, sizeof(wchar_t));
                wcscpy(music_files[mfile_count].path, mplayer->musinfo.locations[i].path);
                wcscat(music_files[mfile_count].path, L"\\");
                wcsncat(music_files[mfile_count].path, fd.cFileName, length_str);

                music_files[mfile_count].altpath = calloc(altpath_len+1, sizeof(char));
                strcpy(music_files[mfile_count].altpath, altpathstr);
                strcat(music_files[mfile_count].altpath, "\\");
                strncat(music_files[mfile_count].altpath, altstr, length_altstr);
                free(altstr); altstr = NULL;
                free(altpathstr); altpathstr = NULL;

                mfile_count++;
                music_files = realloc(music_files, (mfile_count + 1) * sizeof(musloc_t));
                music_files[mfile_count].path = NULL;
                music_files[mfile_count].altpath = NULL;
            } while(FindNextFileW(hfind, &fd));
            FindClose(hfind);
            memset(path_pattern, 0, pathpat_len);
        }
        free(path_pattern);
        #else
        char* path = mplayer->musinfo.locations[i].path;
        DIR* dirp = opendir(path);
        struct dirent* entry = readdir(dirp);
        struct stat sb = {0};
        while(entry) {
            char* ext = strrchr(entry->d_name, '.') + 1;
            size_t total_mpath_size = location_len + strlen(entry->d_name) + 2;
            for(size_t j=0;FILE_EXTENSIONS[j] != NULL;j++) {
                if(strcmp(FILE_EXTENSIONS[j], ext) == 0) {
                    music_files[mfile_count].path = calloc(total_mpath_size, sizeof(char));
                    strcpy(music_files[mfile_count].path, path);
                    strcat(music_files[mfile_count].path, "/");
                    strcat(music_files[mfile_count].path, entry->d_name);
                    mfile_count++;
                    music_files = realloc(music_files, (mfile_count+1) * sizeof(musloc_t));
                    music_files[mfile_count].path = NULL;
                    break;
                }
            }
            entry = readdir(dirp);
        }
        closedir(dirp);
        #endif
    }
    mplayer->musinfo.files = music_files;
    mplayer->musinfo.file_count = mfile_count;
    mplayer->music_count = mfile_count;
}

void mplayer_getmusicpath_info(mplayer_t* mplayer) {
    mplayer_getmusic_locations(mplayer);
    mplayer_getmusic_filepaths(mplayer);
}

#ifdef _WIN32
bool mplayer_musiclocation_exists(mplayer_t* mplayer, wchar_t* location) {
    if(mplayer->musinfo.location_count == 0) {
        mplayer_getmusic_locations(mplayer);
    }
    for(size_t i=0;i<mplayer->musinfo.location_count;i++) {
        if(wcscmp(mplayer->musinfo.locations[i].path, location) == 0) {
            return true;
        }
    }
    return false;
}
#else
bool mplayer_musiclocation_exists(mplayer_t* mplayer, char* location) {
    if(mplayer->musinfo.location_count == 0) {
        mplayer_getmusic_locations(mplayer);
    }
    for(size_t i=0;i<mplayer->musinfo.location_count;i++) {
        if(strcmp(mplayer->musinfo.locations[i].path, location) == 0) {
            return true;
        }
    }
    return false;
}
#endif

#ifdef _WIN32
void mplayer_addmusic_location(mplayer_t* mplayer, wchar_t* location) {
    FILE* f = fopen(MUSIC_PATHINFO_FILE, "a+");
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    rewind(f);
    if(size > 0) {
        if(mplayer_musiclocation_exists(mplayer, location)) {
            fclose(f);
            return;
        }
        fputc('\n', f);
        //wchar_t* newline = L"\n";
        //fwrite(newline, sizeof(wchar_t), wcslen(newline), f);
    }
    char* string = mplayer_widetostring(location);
    fwrite(string, sizeof(char), wcslen(location), f);
    fclose(f);
    mplayer_freemusic_info(mplayer);
    mplayer_getmusicpath_info(mplayer);
}
#else
void mplayer_addmusic_location(mplayer_t* mplayer, char* location) {
    FILE* f = fopen(MUSIC_PATHINFO_FILE, "a+");
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    rewind(f);
    if(size > 0) {
        if(mplayer_musiclocation_exists(mplayer, location)) {
            fclose(f);
            return;
        }
        fputc('\n', f);
    }
    fwrite(location, sizeof(char), strlen(location), f);
    fclose(f);
    mplayer_freemusic_info(mplayer);
    mplayer_getmusicpath_info(mplayer);
}
#endif

void mplayer_loadmusics(mplayer_t* mplayer) {
    music_t* music_list = calloc(mplayer->musinfo.file_count, sizeof(music_t));
    Mix_Music* music = NULL;
    text_info_t utext = {14, NULL, NULL, white, {songs_box.x + 2, songs_box.y + 1}};
    SDL_Rect outer_canvas = utext.text_canvas;
    double music_durationsecs = 0;
    TTF_SetFontSize(mplayer->music_font, utext.font_size);
    #ifdef _WIN32
    TTF_SizeUNICODE(mplayer->music_font, (Uint16*)L"A", &utext.text_canvas.w, &utext.text_canvas.h);
    #else
    TTF_SizeUNICODE(mplayer->music_font, (Uint16*)"A", &utext.text_canvas.w, &utext.text_canvas.h);
    #endif
    
    for(size_t i=0;i<mplayer->musinfo.file_count;i++) {
        #ifdef _WIN32
        char* music_path = mplayer_widetostring(mplayer->musinfo.files[i].path);
        #else
        char* music_path = mplayer->musinfo.files[i].path;
        #endif

        music = Mix_LoadMUS(music_path);
        if(music == NULL) {
            #ifdef _WIN32
            music = Mix_LoadMUS(mplayer->musinfo.files[i].altpath);
            #endif
            if(music == NULL) {
                printf("Failed to load music %ls\n", mplayer->musinfo.files[i].path);
            }
        }
        music_durationsecs = Mix_MusicDuration(music);
        music_list[i].music = music;
        if(music) {
            music_list[i].music_name = mplayer_getmusic_namefrompath(music, mplayer->musinfo.files[i].path);
            #ifdef _WIN32
                utext.utext = music_list[i].music_name;
            #else
                utext.text = music_list[i].music_name;
            #endif
            if(!mplayer->music_renderinit) {
                mplayer->music_renderpos = i;
                mplayer->music_renderinit = true;
            }
            music_list[i].render = true;
            music_list[i].text_texture = mplayer_renderunicode_text(mplayer, mplayer->music_font, &utext);
            outer_canvas.h = utext.text_canvas.h + 22, outer_canvas.w = WIDTH - scrollbar.w;
            utext.text_canvas.x = outer_canvas.x + 50,
            utext.text_canvas.y = outer_canvas.y + ((outer_canvas.h - utext.text_canvas.h) / 2);
            music_list[i].text_info = utext;
            music_list[i].outer_canvas = outer_canvas;
            utext.text_canvas.y += utext.text_canvas.h + 22;
            outer_canvas.y += utext.text_canvas.h + 25;
        }
        music_list[i].music_path = mplayer->musinfo.files[i].path;
        #ifdef _WIN32
        music_list[i].music_alternatepath = mplayer->musinfo.files[i].altpath;
        #endif
        music_list[i].music_position.hrs = 0, music_list[i].music_position.mins = 0,
        music_list[i].music_position.secs = 0;
        music_list[i].music_playing = 0;
        music_list[i].music_durationsecs = music_durationsecs;
        music_list[i].music_duration = mplayer_music_gettime(music_durationsecs);
        #ifdef _WIN32
        free(music_path); music_path = NULL;
        #endif
    }
    mplayer->music_list = music_list;
}
#ifdef _WIN32
int CALLBACK set_browsertitle(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
    if (uMsg == BFFM_INITIALIZED)
      SetWindowTextA(hwnd, "Select Folder");
    return 0;
}
#endif

void mplayer_browsefolder(mplayer_t* mplayer) {
    #ifdef _WIN32
    SDL_SysWMinfo info;
    SDL_GetVersion(&info.version);
    SDL_GetWindowWMInfo(mplayer->window, &info);
    wchar_t foldername[1000] = {0}, folder_path[10000] = {0};
    int img = 0;
    HWND hWnd = info.info.win.window;
    BROWSEINFOW folder_browser;
    folder_browser.hwndOwner = hWnd;
    folder_browser.pidlRoot = NULL;
    folder_browser.pszDisplayName = foldername;
    folder_browser.lpszTitle = L"Choose the folder to search for music";
    folder_browser.ulFlags = BIF_USENEWUI | BIF_RETURNFSANCESTORS;
    folder_browser.lpfn = set_browsertitle;
    folder_browser.lParam = 0;
    folder_browser.iImage = img;
    PIDLIST_ABSOLUTE item_info = SHBrowseForFolderW(&folder_browser);
    if(item_info) {
        if(SHGetPathFromIDListW(item_info, folder_path) == TRUE) {
            mplayer_addmusic_location(mplayer, folder_path);
        }
    }
    #endif
}

#ifdef _WIN32
wchar_t* mplayer_getmusic_namefrompath(Mix_Music* music, wchar_t* path) {
    wchar_t *music_name, *wpath = path, *wfilename = NULL, *wfilename_cp, *wext = NULL;
    int music_type = Mix_GetMusicType(music);
    switch(music_type) {
        case MUS_MP3: wext = L".mp3"; break;
        case MUS_FLAC: wext = L".flac"; break;
        case MUS_OGG: wext = L".ogg"; break;
        case MUS_OPUS: wext = L".opus"; break;
        case MUS_WAV: wext = L".wav"; break;
        default: wext = L".m4a";
    }    
    wfilename = wcsrchr(wpath, L'\\');
    wfilename++;
    
    size_t name_len = wcslen(wfilename);
    size_t ext_index = 0;
    for(size_t i=0;i<name_len;i++) {
        wfilename++;
        if(wcscmp(wfilename, wext) == 0) { ext_index = i; break; }
    }
    if(ext_index == 0) {
        wfilename -= name_len, wfilename_cp = calloc(name_len+1, sizeof(wchar_t));
    } else {
        name_len = ext_index+1, wfilename -= name_len,
        wfilename_cp = calloc(name_len+1, sizeof(wchar_t));
    }
    for(size_t i=0;i<name_len;i++) {
        wfilename_cp[i] = wfilename[i];
    }
    music_name = wfilename_cp;
    return music_name;
}
#else
char* mplayer_getmusic_namefrompath(Mix_Music* music, char* path) {
    char* music_name = NULL, *filename = NULL, *filename_cp, *ext = NULL;
    int music_type = Mix_GetMusicType(music);
    switch(music_type) {
        case MUS_MP3: ext = ".mp3"; break;
        case MUS_FLAC: ext = ".flac"; break;
        case MUS_OGG: ext = ".ogg"; break;
        case MUS_OPUS: ext = ".opus"; break;
        case MUS_WAV: ext = ".wav"; break;
        default: ext = ".m4a";
    }
    filename = strrchr(path, '/');  filename++;
    size_t name_len = strlen(filename);
    size_t ext_index = 0;
    for(size_t i=0;i<name_len;i++) {
        filename++;
        if(strcmp(filename, ext) == 0) { ext_index = i; break; }
    }
    if(ext_index == 0) {
        filename -= name_len, filename_cp = calloc(name_len+1, sizeof(char));
    } else {
        name_len = ext_index+1, filename -= name_len,
        filename_cp = calloc(name_len+1, sizeof(char));
    }
    for(size_t i=0;i<name_len;i++) {
        filename_cp[i] = filename[i];
    }
    music_name = filename_cp;
    return music_name;
}
#endif

mtime_t mplayer_music_gettime(double seconds) {
    mtime_t music_time = {0};

    // Calculate hours
    if(((int)seconds/3600) > 0) {
        music_time.hrs = seconds/3600;
        seconds -= music_time.hrs * 3600;
    }

    // Calculate minutes and seconds
    if(((int)seconds/60) > 0 && ((int)seconds/60) < 60) {
        music_time.mins = seconds/60;
        seconds -= music_time.mins * 60;
    }
    music_time.secs = seconds;
    return music_time;
}

void mplayer_freecurrmusic(mplayer_t* mplayer) {
    Mix_Music* current_music = mplayer->current_music->music;
    Mix_FreeMusic(current_music);
    mplayer->current_music = NULL;
}

void mplayer_freemusic_info(mplayer_t* mplayer) {
    for(size_t i=0;i<mplayer->musinfo.file_count;i++) {
        if(mplayer->music_list) {
            free(mplayer->music_list[i].music_path); mplayer->music_list[i].music_path = NULL;
            free(mplayer->music_list[i].music_name); mplayer->music_list[i].music_name = NULL;
            mplayer->music_list[i].text_texture = NULL;
            mplayer->musinfo.files[i].path = NULL;
            #ifdef _WIN32
            free(mplayer->music_list[i].music_alternatepath); mplayer->music_list[i].music_alternatepath = NULL;
            mplayer->musinfo.files[i].altpath = NULL;
            #endif
            Mix_FreeMusic(mplayer->music_list[i].music);
        }
    }
    for(size_t i=0;i<mplayer->musinfo.location_count;i++) {
        free(mplayer->musinfo.locations[i].path); mplayer->musinfo.locations[i].path = NULL;
    }

    free(mplayer->musinfo.files); mplayer->musinfo.files = NULL;
    free(mplayer->musinfo.locations); mplayer->musinfo.locations = NULL;
    free(mplayer->music_list); mplayer->music_list = NULL;
    mplayer->musinfo.file_count = 0;
    mplayer->musinfo.location_count = 0;
}