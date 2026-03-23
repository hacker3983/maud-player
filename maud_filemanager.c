#include "maud_filemanager.h"
#include "maud_playlistmanager.h"

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
    size_t old_count = location_list->count;
    maud_location_list_addlocation(location_list, path);
    maud_location_list_write_data(location_list);
    for(size_t i=old_count;i<location_list->count;i++) {
        char* location_path = location_list->locations[i].path;
        maud_filelist_t* files = &location_list->locations[i].files;
        for(size_t j=0;j<files->count;j++) {
            maud_file_t* file = &files->files[j];
            music_t new_music = {0};
            maud_filemanager_loadmusic(maud, &new_music,
                location_path, file);
            maud_filemanager_addmusic(maud, &maud->music_list, &maud->music_count,
                &new_music);
        }
    }
   maud_music_lists_sync(maud);
   maud_queue_sync_itemswithmusics(maud, &maud->play_queue);
   maud_playlistmanager_syncwithmusics(maud);
   maud_music_lists_sync_musicitems_id(maud);
   printf("Successfully loaded musics in the new location %s!\n", path);
}

void maud_filemanager_getmusic_locations(maud_t* maud) {
    maud_locationlist_t* location_list = &maud->locations;
    maud_location_list_load_data(location_list);
}

void maud_filemanager_loadmusic(maud_t* maud, music_t* music, char* location_path,
    maud_file_t* file) {
    maud_music_load(maud, music, location_path, file->path, file->altpath);
}

void maud_filemanager_loadmusics(maud_t* maud) {
    maud_locationlist_t* location_list = &maud->locations;
    for(size_t i=0;i<location_list->count;i++) {
        char* location_path = location_list->locations[i].path;
        maud_filelist_t* files = &location_list->locations[i].files;
        for(size_t j=0;j<files->count;j++) {
            maud_file_t* file = &files->files[j];
            music_t new_music = {0};
            maud_filemanager_loadmusic(maud, &new_music,
                location_path, file);
            maud_filemanager_addmusic(maud, &maud->music_list, &maud->music_count,
                &new_music);
        }
    }
    // Before modifiying the music list ensure that the location that was added contains music files
    // to prevent crashing

    maud_music_lists_sync(maud);
    // as long as data is present in the search bar then we set new search equal to true so when we return back to the
    // default menu the music search results gets updated if a music location was removed vice versa.
    /*if(maud->musicsearchbar_data) {
        maud->music_newsearch = true;
        maud->update_searchresults = true;
    }*/
}

void maud_filemanager_addmusic(maud_t* maud, music_t** music_list,
    size_t* music_count, music_t* music) {
    maud_music_list_add(music_list, music_count, music);
}

void maud_filemanager_delmusic_locationindex(maud_t* maud, size_t loc_index) {
    maud_locationlist_t* location_list = &maud->locations;
    if(loc_index >= location_list->count) {
        return;
    }
    char* location_path = location_list->locations[loc_index].path;
    maud_queue_removemusics_bypath(maud, &maud->play_queue, location_path);
    maud_playlistmanager_removemusics_bypath(maud, location_path);
    maud_music_list_removemusics_bypath(&maud->music_list, &maud->music_count,
        location_path);
    maud_music_lists_sync(maud);
    maud_queue_sync_itemswithmusics(maud, &maud->play_queue);
    maud_playlistmanager_syncwithmusics(maud);
    maud_music_lists_sync_musicitems_id(maud);
    maud_location_list_removelocation(location_list, loc_index);
    maud_location_list_write_data(location_list);
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
    maud_music_list_destroy(&maud->music_list, &maud->music_count);
}

void maud_filemanager_freemusicpath_info(maud_t* maud) {
    maud_location_list_destroy(&maud->locations);
}