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
    wchar_t *music_loc = calloc(2, sizeof(wchar_t)), c = L'\0';
    size_t mloc_len = 0, muslist_count = 0;
    while((c = fgetwc(f)) != WEOF) {
        if(c == '\n') {
            music_loclist[muslist_count++].path = music_loc;
            music_loclist = realloc(music_loclist, (muslist_count + 1) * sizeof(musloc_t));
            music_loclist[muslist_count].path = NULL;
            music_loc = calloc(2, sizeof(wchar_t));
            mloc_len = 0;
            continue;
        } 
        music_loc[mloc_len++] = c;
        wchar_t* temp = music_loc;
        music_loc = calloc(mloc_len + 1, sizeof(wchar_t));
        wcsncpy(music_loc, temp, mloc_len);
        free(temp);
    }
    (mloc_len == 0) ? free(music_loc) : (music_loclist[muslist_count++].path = music_loc);
    music_loc = NULL;
    fclose(f);

    // in case the file is empty we free the pointers and set them to NULL
    if(mloc_len == 0) {
        free(music_loclist);
        music_loclist = NULL;
    }
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
        size_t location_len = wcslen(mplayer->musinfo.locations[i].path);
        size_t pathpat_len = location_len + 8;
        #ifdef _WIN32
        _setmode(_fileno(stdout), _O_U16TEXT);
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
                wprintf(L"%ls: %ld\n", fd.cFileName, wcslen(fd.cFileName));
                char *altstr = mplayer_widetostring(fd.cAlternateFileName),
                    *altpathstr = mplayer_widetostring(mplayer->musinfo.locations[i].path);
                size_t length_str = wcslen(fd.cFileName), length_altstr = wcslen(fd.cAlternateFileName);
                size_t path_len = location_len + length_str + 7,
                    altpath_len = location_len + length_altstr + 7;

                music_files[mfile_count].path = calloc(path_len, sizeof(wchar_t));
                wcscpy(music_files[mfile_count].path, mplayer->musinfo.locations[i].path);
                wcscat(music_files[mfile_count].path, L"\\");
                wcsncat(music_files[mfile_count].path, fd.cFileName, length_str);

                music_files[mfile_count].altpath = calloc(altpath_len, sizeof(char));
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
        DIR* dirp = opendir(mplayer->musinfo.locations[i].path);
        struct dirent* entry = readdir(dirp);
        struct stat sb = {0};
        while(entry) {
            bool valid_musicfile = false;
            char* ext = strrchr(entry->d_name, '.') + 1;
            size_t total_mpath_size = location_len + strlen(entry->d_name) + 2;
            // use stat instead of d_type to make more filesystem independent
            stat(entry->d_name, &sb);
            for(size_t j=0;FILE_EXTENSIONS[j] != NULL;j++) {
                if(strcmp(FILE_EXTENSIONS[j], ext) == 0) {
                    valid_musicfile = true;
                    break;
                }
            }
            if(valid_musicfile) {
                music_files[mfile_count].path = calloc(total_mpath_size, sizeof(char));
                strcpy(music_files[mfile_count].path, mplayer->musinfo.locations[i].path);
                strcat(music_files[mfile_count].path, "/");
                strcat(music_files[mfile_count].path, entry->d_name);
                mfile_count++;
                music_files = realloc(music_files, (mfile_count+1) * sizeof(musloc_t));
                music_files[mfile_count].path = NULL;
            }
            entry = readdir(dirp);
        }
        closedir(dirp);
        #endif
    }
    printf("Here\n");
    mplayer->musinfo.files = music_files;
    mplayer->musinfo.file_count = mfile_count;
    mplayer->music_count = mfile_count;
}

void mplayer_getmusicpath_info(mplayer_t* mplayer) {
    mplayer_getmusic_locations(mplayer);
    mplayer_getmusic_filepaths(mplayer);
}

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
        fputs("\n", f);
    }
    fwrite(location, sizeof(wchar_t), wcslen(location), f);
    fclose(f);
    mplayer_freemusic_info(mplayer);
    mplayer_getmusicpath_info(mplayer);
}

void mplayer_loadmusics(mplayer_t* mplayer) {
    music_t* music_list = calloc(mplayer->musinfo.file_count, sizeof(music_t));
    Mix_Music* music = NULL;
    double music_durationsecs = 0;
    for(size_t i=0;i<mplayer->musinfo.file_count;i++) {
        char* music_path = mplayer_widetostring(mplayer->musinfo.files[i].path);
        music = Mix_LoadMUS(music_path);
        if(music == NULL) {
            music = Mix_LoadMUS(mplayer->musinfo.files[i].altpath);
            if(music == NULL) {
                printf("Failed to load music %s\n", mplayer->musinfo.files[i].path);
            }
        }
        free(music_path); music_path = NULL;

        music_durationsecs = Mix_MusicDuration(music);
        music_list[i].music = music;
        if(music) {
            music_list[i].music_name = mplayer_getmusic_namefrompath(music, mplayer->musinfo.files[i].path);
        }
        music_list[i].music_path = mplayer->musinfo.files[i].path;
        music_list[i].music_alternatepath = mplayer->musinfo.files[i].altpath;
        music_list[i].music_position.hrs = 0, music_list[i].music_position.mins = 0,
        music_list[i].music_position.secs = 0;
        music_list[i].music_playing = 0;
        music_list[i].music_durationsecs = music_durationsecs;
        music_list[i].music_duration = mplayer_music_gettime(music_durationsecs);
    }
    mplayer->music_list = music_list;
}
int CALLBACK set_browsertitle(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
    if (uMsg == BFFM_INITIALIZED)
      SetWindowTextA(hwnd, "Select Folder");
    return 0;
}

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

wchar_t* mplayer_getmusic_namefrompath(Mix_Music* music, wchar_t* path) {
    wchar_t* music_name = NULL, *filename = NULL, *ext = NULL,
        *wpath = path, *wfilename = NULL, *wext = NULL;
    int music_type = Mix_GetMusicType(music);
    _setmode(_fileno(stdout), _O_U16TEXT);
    switch(music_type) {
        case MUS_MP3:
            wext = L".mp3";
            break;
        case MUS_FLAC:
            wext = L".flac";
            break;
        case MUS_OGG:
            wext = L".ogg";
            break;
        case MUS_OPUS:
            wext = L".opus";
            break;
        case MUS_WAV:
            wext = L".wav";
            break;
        default:
            wext = L".m4a";
    }
    
    #ifdef _WIN32
    wfilename = wcsrchr(wpath, L'\\');
    #else
    filename = strrchr(path, '/');
    #endif
    //wprintf(L"extension: %ls\n", wext);
    wfilename++;
    
    //printf("%ls, %ls\n", wfilename, wpath);
    size_t name_len = wcslen(wfilename);
    filename = calloc(name_len, sizeof(wchar_t));
    size_t ext_index = 0;
    for(size_t i=0;i<name_len;i++) {
        wfilename++;
        if(wcscmp(wfilename, wext) == 0) {
            ext_index = i; break;
        }
    }
    wfilename -= (ext_index+1);
    //wprintf(L"%ld\n", ext_index);
    for(size_t i=0;i<ext_index+1;i++) {
        filename[i] = wfilename[i];
    }
    if(ext_index == 0) {
        printf("No extension found!\n");
        wfilename -= name_len;
        wfilename++;
        for(size_t i=0;i<name_len;i++) {
            filename[i] = wfilename[i];
        }
    }
    //wprintf(L"%ls\n", wfilename);
    music_name = filename;
    /*printf("musicname_length: %ld\n", name_len);
    //strncpy(music_name, filename, name_len);

    wfilename--;*/
    return music_name;
}

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
        free(mplayer->musinfo.files[i].path);
        if(mplayer->music_list) {
            free(mplayer->music_list[i].music_name);
            mplayer->music_list[i].music_name = NULL;
            Mix_FreeMusic(mplayer->music_list[i].music);
        }
    }
    for(size_t i=0;i<mplayer->musinfo.location_count;i++) {
        free(mplayer->musinfo.locations[i].path);
    }

    free(mplayer->musinfo.files); mplayer->musinfo.files = NULL;
    free(mplayer->musinfo.locations); mplayer->musinfo.locations = NULL;
    free(mplayer->music_list); mplayer->music_list = NULL;
    mplayer->musinfo.file_count = 0;
    mplayer->musinfo.location_count = 0;
}