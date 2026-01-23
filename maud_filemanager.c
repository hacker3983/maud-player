#include "maud_filemanager.h"

#ifdef _WIN32
void maud_filemanager_getroot_path(char* root_path) {
        size_t directory_length = GetCurrentDirectoryA(0, NULL);
        char* directory = calloc(directory_length, sizeof(char));
        GetCurrentDirectoryA(directory_length, directory);
        SetCurrentDirectoryA("/");
        GetCurrentDirectoryA(4, root_path);
        SetCurrentDirectoryA(directory);
        free(directory); directory = NULL;
}
#endif

void maud_filemanager_addmusic_location(maud_t* maud, const char* path) {
    maud_locationlist_t* location_list = &maud->locations;
    maud_location_list_addlocation(location_list, path);
    maud_location_list_write_data(location_list);
}

void maud_filemanager_getmusic_locations(maud_t* maud) {
    maud_locationlist_t* location_list = &maud->locations;
    maud_location_list_load_data(location_list);
}

void maud_filemanager_getmusic_filepaths(maud_t* maud) {
    maud_locationlist_t* location_list = &maud->locations;
    if(!location_list->locations) {
        return;
    }
    maud->total_filecount = 0;
    for(size_t i=0;i<location_list->count;i++) {
        maud_location_t* location = &location_list->locations[i];
        maud_file_list_load_files(&location->files, location->path);
        maud->total_filecount += location->files.count;
    }
}

void maud_filemanager_getmusicpath_info(maud_t* maud) {
    maud_filemanager_getmusic_locations(maud);
    maud_filemanager_getmusic_filepaths(maud);
}

void maud_filemanager_loadmusics(maud_t* maud) {
    maud_locationlist_t* location_list = &maud->locations;
    music_t* music_list = NULL;
    size_t music_count = 0;
    Mix_Music* music = NULL;
    text_info_t utext = {18, NULL, NULL, white, {songs_box.x + 2, songs_box.y + 1}};
    SDL_Rect outer_canvas = utext.text_canvas;
    double music_durationsecs = 0;
    if(!maud->music_list) {
        music_list = calloc(maud->total_filecount, sizeof(music_t));
        // As long as the music count is greater than zero then we save the maud->music_list
        for(size_t i=0;i<maud->music_count;i++) {
            music_list[i] = maud->music_list[i];
        }
    }
    for(size_t i=0;i<location_list->count;i++) {
        maud_filelist_t* files = &location_list->locations[i].files;
        for(size_t j=0;j<files->count;j++) {
        
        }
    }
}

void maud_filemanager_delmusic_locationindex(maud_t* maud, size_t loc_index) {
    maud_locationlist_t* location_list = &maud->locations;
    if(loc_index >= location_list->count) {
        return;
    }
    char* location_path = location_list->locations[loc_index].path;
    maud_markmusic_removalbypath(maud, location_path);
    maud_location_list_removelocation(location_list, loc_index);
    maud_location_list_write_data(location_list);
}


void maud_filemanager_markmusic_removalby_locationpath(maud_t* maud,
    const char* location_path) {
    music_t* music_list = &maud->music_list;
    for(size_t i=0;i<maud->music_count;i++) {
        if(strcmp(music_list[i].path, location_path) == 0) {
            music_list[i].remove = true;
        }
    }
}

void maud_filemanager_loadmusics(maud_t* maud) {
    if(!maud->locations.locations) {
        maud_filemanager_freemusic_list(maud);
        return;
    }
}

const char* maud_filemanager_getextension_name(Mix_Music* music) {
    int music_type = Mix_GetMusicType(music);
    const char* pextension_name = NULL;
    static char extension_name[5] = {0};
    switch(music_type) {
        case MUS_MP3:  pextension_name = FILE_EXTENSIONS[MAUD_MP3EXT];  break;
        case MUS_FLAC: pextension_name = FILE_EXTENSIONS[MAUD_FLACEXT]; break;
        case MUS_OGG:  pextension_name = FILE_EXTENSIONS[MAUD_OGGEXT];  break;
        case MUS_OPUS: pextension_name = FILE_EXTENSIONS[MAUD_OPUSEXT]; break;
        case MUS_WAV:  pextension_name = FILE_EXTENSIONS[MAUD_WAVEXT];  break;
        default:       pextension_name = FILE_EXTENSIONS[MAUD_M4AEXT];
    }
    strcpy(extension_name, pextension_name);
    return extension_name;
}

char* maud_filemanager_getmusic_namefrompath(Mix_Music* music, const char* path) {
    if(path == NULL) {
        return NULL;
    }
    const char* extension_name = maud_filemanager_getextension_name(music);
    char *music_name = NULL, *filename = NULL;
    #ifdef _WIN32
    //filename = maud_widetoutf8(wcsrchr((wchar_t*)path, L'\\'));
    filename = strrchr(path, "\\");
    #else
    filename = strrchr(path, '/');
    #endif
    // Skip forward slash or backslash
    filename++;
    // Calculate music name length by subtracting extension length
    size_t extension_len = strlen(extension_name) + 1,
           filename_len = strlen(filename),
           music_namelen = filename_len - extension_len;
    if(!music_namelen) {
        music_namelen = filename_len;
    }
    // Allocate memory and copy the music name into the allocated pointer
    music_name = calloc(music_namelen + 1, sizeof(char*));
    strncpy(music_name, filename, music_namelen);
    return music_name;
}


#ifdef _WIN32
int CALLBACK set_browsertitle(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
    if (uMsg == BFFM_INITIALIZED)
      SetWindowTextA(hwnd, "Select Folder");
    return 0;
}
#endif

void maud_filemanager_browsefolder(maud_t* maud) {
    #ifdef _WIN32
    SDL_SysWMinfo info;
    SDL_GetVersion(&info.version);
    SDL_GetWindowWMInfo(maud->window, &info);
    wchar_t foldername[MAX_PATH+1] = {0}, folder_path[MAX_PATH+1] = {0};
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
        printf("folder_browser.pzDisplayName = %ls\n", foldername);
        if(SHGetPathFromIDListW(item_info, folder_path) == TRUE) {
            printf("folder_path: %ls\n", folder_path);
            char* folderpath_str = maud_widetoutf8(folder_path);
            maud_filemanager_addmusic_location(maud, folderpath_str);
            free(folderpath_str);
        }
    }
    #else
    FILE* f = popen("zenity --file-selection --directory --title \"Select a directory to search for music\"", "r");
    size_t default_chunksize = 1024, dirlen = 0, current_chunksize = 1024, old_chunksize = 0;
    char* directory = NULL, c = 0;
    while((c = fgetc(f)) != EOF) {
        if(c == '\n') {
            break;
        }
        if(!directory) {
            directory = calloc(current_chunksize, sizeof(char));
        } else if(dirlen == current_chunksize) {
            old_chunksize = current_chunksize;
            current_chunksize += default_chunksize;
            directory = realloc(directory,  current_chunksize * sizeof(char));
            memset((char*)directory + old_chunksize, 0, current_chunksize - old_chunksize);
        }
        directory[dirlen++] = c;
    }
    /* ensure that dirlen is greater than zero before adding the location incase the user presses the cancel button
       in zenity.
    */
    if(dirlen > 0) {
        maud_filemanager_addmusic_location(maud, directory);
    }
    free(directory);
    #endif
}

mtime_t maud_filemanager_music_gettime(double seconds) {
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

void maud_filemanager_freemusic(music_t* music_ref) {
    // release the memory used for the music name, music name texture, and other music related data that was
    // allocaterd in memory
    SDL_DestroyTexture(music_ref->text_texture); music_ref->text_texture = NULL;
    free(music_ref->music_name); music_ref->music_name = NULL;
    music_ref->path = NULL;
    Mix_FreeMusic(music_ref->music); music_ref->music = NULL;
}

void maud_filemanager_freemusic_searchresult(music_t** music_searchresult, size_t* music_resultcount) {
    if(!(*music_searchresult)) {
        return;
    }
    for(size_t i=0;i<*music_resultcount;i++) {
        free((*music_searchresult)[i].music_name);
        SDL_DestroyTexture((*music_searchresult)[i].text_texture);
        (*music_searchresult)[i].music_name = NULL;
        (*music_searchresult)[i].text_texture = NULL;
    }
    free(*music_searchresult); *music_searchresult = NULL;
    *music_resultcount = 0;
}

void maud_filemanager_freemusic_list(maud_t* maud) {
    /* Release the memory used by the music list back to the system */
    for(size_t i=0;i<maud->music_count;i++) {
        maud_filemanager_freemusic(&maud->music_list[i]);
    }
    //maud_filemanager_freemusic_searchresult(&maud->music_searchresult, &maud->music_searchresult_count);
    free(maud->music_list); maud->music_list = NULL; maud->music_count = 0;
    maud->musicpending_removalcount = 0;
    maud->music_renderpos = 0;
    maud->music_maxrenderpos = 0;
    //maud->match_maxrenderpos = 0;
    //maud->music_searchrenderpos = 0;
}

void maud_filemanager_freemusicpath_info(maud_t* maud) {
    maud_location_list_destroy(&maud->locations);
}