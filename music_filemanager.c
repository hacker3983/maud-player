#include "music_filemanager.h"

#ifdef _WIN32
void mplayer_filemanager_getroot_path(char* root_path) {
        size_t directory_length = GetCurrentDirectoryA(0, NULL);
        char* directory = calloc(directory_length, sizeof(char));
        GetCurrentDirectoryA(directory_length, directory);
        SetCurrentDirectoryA("/");
        GetCurrentDirectoryA(4, root_path);
        SetCurrentDirectoryA(directory);
        free(directory); directory = NULL;
}
#endif

void mplayer_filemanager_getmusic_locations(mplayer_t* mplayer) {
    FILE* f = fopen(MUSIC_PATHINFO_FILE, "r");
    if(f == NULL) {
        printf("Error: %s file doesn't exist: Creating %s file...\n", MUSIC_PATHINFO_FILE, MUSIC_PATHINFO_FILE);
        mplayer->music_count = 0;
        mplayer->music_list = NULL;
        mplayer->locations = NULL;
        mplayer->location_count = 0;
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
        if(wc == L'\n' && mloc_len > 0) {
            music_loclist[muslist_count++].path = music_loc;
            music_loclist = realloc(music_loclist, (muslist_count + 1) * sizeof(musloc_t));
            music_loclist[muslist_count].path = NULL;
            music_loclist[muslist_count].file_count = 0;
            music_loc = calloc(2, sizeof(wchar_t));
            mloc_len = 0;
            continue;
        } else if(wc == L'\n' && mloc_len == 0) {
            continue;
        }
        music_loc[mloc_len++] = wc;
        wchar_t* newptr = calloc(mloc_len + 1, sizeof(wchar_t));
        wcsncpy(newptr, music_loc, mloc_len);
        free(music_loc); music_loc = newptr;
        #else
        if(c == '\n' && mloc_len > 0) {
            music_loclist[muslist_count++].path = music_loc;
            music_loclist = realloc(music_loclist, (muslist_count + 1) * sizeof(musloc_t));
            music_loclist[muslist_count].path = NULL;
            music_loc = calloc(2, sizeof(char));
            mloc_len = 0;
            continue;
        } else if(c == '\n' && mloc_len == 0) {
            continue;
        }
        music_loc[mloc_len++] = c;
        char* newptr = calloc(mloc_len + 1, sizeof(char));
        strncpy(newptr, music_loc, mloc_len);
        free(music_loc); music_loc = newptr;
        #endif
    }
    // Whenever the MUSIC_PATHINFO_FILE is empty release the memory of the music_loclist back to the system
    if(muslist_count == 0 && mloc_len == 0) {
        free(music_loclist); music_loclist = NULL;
    }
    /* If the last line of the file is reached and it does not encounter a newline character while it was iterating
       in the loop then we add it to the list and if the music location length is equal to zero we release the memory
       to the system.
    */
    (mloc_len == 0) ? free(music_loc) : (music_loclist[muslist_count++].path = music_loc);
    mplayer->locations = music_loclist;
    mplayer->location_count = muslist_count;
}

void mplayer_filemanager_getmusic_filepaths(mplayer_t* mplayer) {
    // If there are no music locations present in the file then we exit the function
    if(mplayer->locations == NULL) {
        return;
    }
    // Initialize the total_filecount by setting it to zero and the list that will contain the music location files
    mplayer->total_filecount = 0;
    musloc_t* musiclocation_files = calloc(1, sizeof(musloc_t));
    size_t musicloc_filecount = 0;
    for(size_t i=0;i<mplayer->location_count;i++) {
        #ifdef _WIN32
        size_t location_len = wcslen(mplayer->locations[i].path);
        size_t pathpat_len = location_len + 8;
        #else
        size_t location_len = strlen(mplayer->locations[i].path);
        #endif
        #ifdef _WIN32
        if(!PathFileExistsW(mplayer->locations[i].path)) {
            LPSTR messageBuffer = NULL;
            size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
                | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPSTR)&messageBuffer, 0, NULL);
            char* path = mplayer_widetoutf8(mplayer->locations[i].path);
            fprintf(stderr, "Error: The music location %s: %s", path, messageBuffer);
            free(path); path = NULL;
            LocalFree(messageBuffer);
            continue;
        }
        wchar_t* path_pattern = calloc(pathpat_len, sizeof(wchar_t));
        for(int j=0;FILE_EXTENSIONS[j] != NULL;j++) {
            wcscpy(path_pattern, mplayer->locations[i].path);
            wcscat(path_pattern, L"\\*.");
            wcscat(path_pattern, FILE_EXTENSIONS[j]);

            WIN32_FIND_DATAW fd = {0};
            HANDLE hfind = FindFirstFileW(path_pattern, &fd);
            if(hfind == INVALID_HANDLE_VALUE) {
                continue;
            }
            do {
                char *altstr = mplayer_widetoutf8(fd.cAlternateFileName),
                    *altpathstr = mplayer_widetoutf8(mplayer->locations[i].path);
                size_t length_str = wcslen(fd.cFileName), length_altstr = wcslen(fd.cAlternateFileName);
                size_t path_len = location_len + length_str + 7,
                    altpath_len = location_len + length_altstr + 7;

                musiclocation_files[musicloc_filecount].path = calloc(path_len+1, sizeof(wchar_t));
                wcscpy(musiclocation_files[musicloc_filecount].path, mplayer->locations[i].path);
                wcscat(musiclocation_files[musicloc_filecount].path, L"\\");
                wcsncat(musiclocation_files[musicloc_filecount].path, fd.cFileName, length_str);

                musiclocation_files[musicloc_filecount].altpath = calloc(altpath_len+1, sizeof(char));
                strcpy(musiclocation_files[musicloc_filecount].altpath, altpathstr);
                strcat(musiclocation_files[musicloc_filecount].altpath, "\\");
                strncat(musiclocation_files[musicloc_filecount].altpath, altstr, length_altstr);
                free(altstr); altstr = NULL;
                free(altpathstr); altpathstr = NULL;
                musiclocation_files[musicloc_filecount].location_index = mplayer->total_filecount;
                musicloc_filecount++; mplayer->total_filecount++;
                musiclocation_files = realloc(musiclocation_files, (musicloc_filecount + 1) *
                                                sizeof(musloc_t));
                musiclocation_files[musicloc_filecount].path = NULL;
                musiclocation_files[musicloc_filecount].altpath = NULL;
                musiclocation_files[musicloc_filecount].file_count = 0;
            } while(FindNextFileW(hfind, &fd));
            FindClose(hfind);
            memset(path_pattern, 0, pathpat_len);
        }
        free(path_pattern);
        #else
        char* path = mplayer->locations[i].path;
        DIR* dirp = opendir(path);
        if(!dirp) {
            fprintf(stderr, "Error: The music location %s: %s\n", path, strerror(errno));
            continue;
        }
        struct dirent* entry = readdir(dirp);
        struct stat sb = {0};
        while(entry) {
            char* ext = strrchr(entry->d_name, '.');
            if(!ext) {
                entry = readdir(dirp); continue;
            }
            ext++;
            size_t total_mpath_size = location_len + strlen(entry->d_name) + 2;
            for(size_t j=0;FILE_EXTENSIONS[j] != NULL;j++) {
                if(strcmp(FILE_EXTENSIONS[j], ext) == 0) {
                    musiclocation_files[musicloc_filecount].path = calloc(total_mpath_size, sizeof(char));
                    strcpy(musiclocation_files[musicloc_filecount].path, path);
                    strcat(musiclocation_files[musicloc_filecount].path, "/");
                    strcat(musiclocation_files[musicloc_filecount].path, entry->d_name);
                    musiclocation_files[musicloc_filecount].location_index = mplayer->total_filecount;
                    musicloc_filecount++; mplayer->total_filecount++;
                    musiclocation_files = realloc(musiclocation_files, (musicloc_filecount+1) * sizeof(musloc_t));
                    musiclocation_files[musicloc_filecount].path = NULL;
                    break;
                }
            }
            entry = readdir(dirp);
        }
        closedir(dirp);
        #endif
        mplayer->locations[i].files = musiclocation_files;
        mplayer->locations[i].file_count = musicloc_filecount;
        mplayer->locations[i].render = true;
        musiclocation_files = NULL, musiclocation_files = calloc(1, sizeof(musloc_t));
        musicloc_filecount = 0;
    }
}

void mplayer_filemanager_getmusicpath_info(mplayer_t* mplayer) {
    mplayer_filemanager_getmusic_locations(mplayer);
    mplayer_filemanager_getmusic_filepaths(mplayer);
}

bool mplayer_filemanager_musiclocation_exists(mplayer_t* mplayer, void* locationv) {
    if(mplayer->location_count == 0) {
        mplayer_filemanager_getmusic_locations(mplayer);
    }
    #ifdef _WIN32
    wchar_t* location = (wchar_t*)locationv;
    #else
    char* location = (char*)locationv;
    #endif
    for(size_t i=0;i<mplayer->location_count;i++) {
        #ifdef _WIN32
        printf("location: %ls\n", location);
        if(wcscmp(mplayer->locations[i].path, location) == 0) {
            return true;
        }
        #else
        if(strcmp(mplayer->locations[i].path, location) == 0) {
            return true;
        }
        #endif
    }
    return false;
}

void mplayer_filemanager_addmusic_location(mplayer_t* mplayer, void* locationv) {
    FILE* f = fopen(MUSIC_PATHINFO_FILE, "a+");
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f), location_length = 0;
    char* location = NULL;
    rewind(f);
    
    // if the MUSIC PATH INFO FILE is not empty we check to see if the location exists
    // whenever it exists we just close the file stream and exit the function otherwise we just add a new line
    // to accomodate the location we are going to add to the music path info file
    if(size > 0) {
        if(mplayer_filemanager_musiclocation_exists(mplayer, locationv)) {
            fclose(f);
            return;
        }
        fputc('\n', f);
        //wchar_t* newline = L"\n";
        //fwrite(newline, sizeof(wchar_t), wcslen(newline), f);
    }

    // whenever on windows we use wchar_t to store the unicode data
    // on any other platform we use just a regular char*
    #ifdef _WIN32
    location = mplayer_widetoutf8((wchar_t*)locationv);
    location_length = wcslen((wchar_t*)locationv);
    #else
    location = (char*)locationv;
    location_length = strlen(location);
    #endif

    printf("Adding music location location %s:\n", location);

    // write the location to the MUSIC PATHS FILE and close it
    fwrite(location, sizeof(char), location_length, f);
    fclose(f);

    // whenever on windows we allocate memory on the heap to convert the location into char* so we free the memory
    // when we are done with it
    #ifdef _WIN32
    free(location); location = NULL;
    #endif

    // reload the music locations, also the music list
    mplayer_filemanager_freemusicpath_info(mplayer);
    mplayer_filemanager_getmusicpath_info(mplayer);
    mplayer->music_locationadded = true;
    mplayer_filemanager_loadmusics(mplayer);
    mplayer->music_locationadded = false;
    printf("Successfully added music locations and reloaded the musics:\n");
    printf("mplayer->total_filecount: %zu, mplayer->music_count is %zu\n", mplayer->total_filecount, mplayer->music_count);
}

void mplayer_filemanager_delmusic_locationindex(mplayer_t* mplayer, size_t loc_index) {
    // reset the music pending removal count to zero before calling the del music location index
    // to prevent curruption or segmentation faults
    mplayer->musicpending_removalcount = 0;
    if(loc_index >= mplayer->location_count) {
        return;
    }
    // Open the MUSIC PATH INFO FILE in write mode
    FILE* f = fopen(MUSIC_PATHINFO_FILE, "w+");
    char* locations_buff = NULL;
    size_t size = 0;
    // iterate through the location list
    for(size_t i=0;i<mplayer->location_count;i++) {
        // whenever the current location index is equal to the location index that should be removed
        // we set the remove status of the musics in that particular location to true as long as this condition is true
        // we skip over the music index that should be removed in the location list
        if(i == loc_index) {
            for(size_t j=0;j<mplayer->locations[i].file_count;j++) {
                // retrieve the index of the music in the location that should be removed
                // and set the remove status to true
                size_t music_index = mplayer->locations[i].files[j].location_index;
                mplayer->music_list[music_index].remove = true;
                if(mplayer->music_list[music_index].checkbox_ticked) {
                    mplayer->tick_count--;
                }
            }
            // store the amount of musics that should be removed at that particular location
            // in the variable mplayer->musicpending_removalcount
            mplayer->musicpending_removalcount = mplayer->locations[i].file_count;
            continue;
        }

        // retrieve the length of the music locations that are not gonna be removed
        #ifdef _WIN32
        // convert from wchar_t to multibyte string path when on windows
        size_t len = wcslen(mplayer->locations[i].path);
        char* loc_str = mplayer_widetoutf8(mplayer->locations[i].path);
        #else
        size_t len = strlen(mplayer->locations[i].path);
        char* loc_str = mplayer->locations[i].path;
        #endif
        // once we get the length of that particular music location we increment the amount of bytes that will be
        // added to the MUSICPATHS.info file
        size += len + 1;
        // whenever the locations_buff variable is NULL then we allocate memory to store the locations in the
        // locations_buff variable
        if(!locations_buff) {
            // allocate memory to store the location plus a null byte to separate the location
            locations_buff = calloc(size+1, sizeof(char));
            strncpy(locations_buff, loc_str, len); // actually copy the location into the locations_buff
            if(i+1 == loc_index && i+1 == mplayer->location_count-1) {
                /* whenever the current location index plus 1 is equal to the location index and it is equal to the
                   location_count-1 then that means we should set the locations_buff[size-1] equal to NULL since the
                   location index that should be removed will be at the end
                */
                locations_buff[size-1] = '\0';
                size--;
            } else if(i < loc_index && i+1 != mplayer->location_count) {
                /* whenever the current location index is less than the location index and i+1 is not equal the location_count
                   that means we have more locations before the actually location that should be removed so we separate the
                   locations with a \n or 0xa byte by concatenating with strcat function
                */
                strcat(locations_buff, "\n");
            } else if(i > loc_index && i+1 != mplayer->location_count) {
                /* whenever the current location index is greater than the loc_index and the current location index + 1
                   is not equal to the location count that means the there are locations that come after the location index
                   that should be removed therefore we seperate the current location with a new line character
                */
                strcat(locations_buff, "\n");
            } else {
                /* whenever none of the conditions above is true we just decrement the size in bytes of the total that
                   will be written to the MUSIC PATHS info file
                */
                size--;
            }
            continue;
        }
        // reallocate memory to store any more locations that will be written to the file
        locations_buff = realloc(locations_buff, (size+1) * sizeof(char));
        // set the NULL byte at the end of the string
        locations_buff[size] = '\0';
        // actually place the new location string into the buffer
        strncat(locations_buff, loc_str, len);

        if(i+1 == loc_index && i+1 == mplayer->location_count-1) {
            locations_buff[size-1] = '\0';
            size--;
        } else if(i < loc_index && i != mplayer->location_count) {
            locations_buff[size-1] = '\n';
        } else if(i > loc_index && i+1 != mplayer->location_count) {
            locations_buff[size-1] = '\n';
        } else {
            locations_buff[size-1] = '\0';
            size--;
        }
        #ifdef _WIN32
        free(loc_str); loc_str = NULL;
        #endif
    }

    // write the locations buffer that was create to the MUSICPATHS INFO file and close the file stream
    // and deallocate the allocated locations buff
    fwrite(locations_buff, sizeof(char), size, f);
    fclose(f);
    free(locations_buff); locations_buff = NULL;
    // Reload the music paths and the music after deleting the location
    mplayer_filemanager_freemusicpath_info(mplayer);
    mplayer_filemanager_getmusicpath_info(mplayer);
    // whenever the location that was removed contains music then we actually reload the musics in the music list
    // otherwise the music list remains constant or unchanged.
    if(mplayer->musicpending_removalcount > 0) {
        mplayer->music_locationremoved = true;
        mplayer_filemanager_loadmusics(mplayer);
        // Whenever for example a music location is removed and the music count becomes zero then we set the selection menu
        // check all button clicked and ticked status to false;
        if(mplayer->music_selectionmenu_checkbox_tickall && !mplayer->music_count) {
            mplayer->music_selectionmenu_checkbox_fillall = false;
            mplayer->music_selectionmenu_checkbox_tickall = false;
            mplayer->music_selectionmenu_checkbox_clicked = false;
        }
        mplayer->music_locationremoved = false;
    }
}

void mplayer_filemanager_copymusicinfo_fromsearchindex(mplayer_t* mplayer, size_t index, music_t* music_info) {
    Mix_Music* music = mplayer->music_list[index].music;
    text_info_t utext = {14, NULL, NULL, white, {songs_box.x + 2, songs_box.y + 1}};
    SDL_Rect outer_canvas = utext.text_canvas;
    double music_durationsecs = 0;

    music_durationsecs = Mix_MusicDuration(music);
    music_info->searchmusic_id = index;

    /* Render the music_name and initialize the other infos (positions, texture, etc) */
    music_info->music_name = mplayer_filemanager_getmusic_namefrompath(music, mplayer->music_list[index].music_path);
    music_info->text_texture = mplayer_textmanager_renderunicode(mplayer, mplayer->music_font,
        &mplayer->music_list[index].text_info);
    music_info->music_path = mplayer->music_list[index].music_path;
    #ifdef _WIN32
    music_info->music_alternatepath = mplayer->music_list[index].music_alternatepath;
    #endif
    music_info->music_position.hrs = 0, music_info->music_position.mins = 0,
    music_info->music_position.secs = 0;
    music_info->music_durationsecs = music_durationsecs;
    music_info->music_duration = mplayer_filemanager_music_gettime(music_durationsecs);
    music_info->search_match = mplayer->music_list[index].search_match;
    music_info->search_render = mplayer->music_list[index].search_render;
    music_info->text_info = mplayer->music_list[index].text_info;
    music_info->outer_canvas = mplayer->music_list[index].outer_canvas;
    music_info->render = mplayer->music_list[index].render;
    music_info->checkbox_ticked = false;
    music_info->fill = false;
}

void mplayer_filemanager_copymusic_info(music_t* src_info, music_t* dest_info) {
    dest_info->checkbox_size = src_info->checkbox_size;
    dest_info->checkbox_ticked = src_info->checkbox_ticked;
    dest_info->fill = src_info->fill;
    dest_info->music = src_info->music;
    dest_info->music_position = src_info->music_position;
    dest_info->music_duration = src_info->music_duration;
    dest_info->music_durationsecs = src_info->music_durationsecs;
    dest_info->render = src_info->render;
    dest_info->search_render = src_info->search_render;
    dest_info->music_name = src_info->music_name;
    dest_info->text_info = src_info->text_info;
    dest_info->outer_canvas = src_info->outer_canvas;
    dest_info->text_texture = src_info->text_texture;
    #ifdef _WIN32
    dest_info->music_alternatepath = src_info->music_alternatepath;
    #endif
}

void mplayer_filemanager_removemusics_pendingremoval(mplayer_t* mplayer) {
    if(!mplayer->musicpending_removalcount) {
        return;
    }
    // if music_count is equal to zero then we exit the function
    if(mplayer->music_count == 0) {
        return;
    }
    text_info_t utext = {14, NULL, NULL, white, {songs_box.x + 2, songs_box.y + 1}};
    SDL_Rect outer_canvas = utext.text_canvas;
    // determine the new music count after removing the musics it would basically be equal to the total_filecount
    // in mplayer_structure
    size_t newmusic_count = mplayer->music_count - mplayer->musicpending_removalcount, j = 0, new_playid = 0;
    bool preserve_playid = false;

    // create a new list to store the musics that were not marked for removal
    music_t *newmusic_list = (newmusic_count == 0) ? NULL : calloc(newmusic_count, sizeof(music_t)),
            *oldmusic_list = NULL;
    size_t new_locationindex = 0, new_location_filecount = 0;
    for(size_t i=0;i<mplayer->music_count;i++) {
        // As long as the new music list is NULL we exit the loop
        if(!newmusic_list) {
            mplayer->music_renderpos = 0;
            mplayer->music_maxrenderpos = 0;
            break;
        }
        if(i+1 >= mplayer->music_renderpos && mplayer->music_list[i].remove) {
            printf("i+1 is greater than the music_renderpos\n");
            /* Whenever the the music index i is equal to the current render position or scroll bar position
               we set the current music render position equal to the new music render position j this will
               allow the user to maintain there scroll position without it resetting every time a music location
               is removed
            */
            if(i+1 == mplayer->music_renderpos) {
                mplayer->music_renderpos = j+1;
            }
            // calculate the canvas positions and dimensions for the current music that will not be removed
            utext = mplayer->music_list[i].text_info;
            outer_canvas.h = utext.text_canvas.h + 22, outer_canvas.w = WIDTH - scrollbar.w;
            utext.text_canvas.x = outer_canvas.x + 50,
            utext.text_canvas.y = outer_canvas.y + ((outer_canvas.h - utext.text_canvas.h) / 2);
    
            mplayer->music_list[i].text_info.text_canvas = utext.text_canvas;
            mplayer->music_list[i].outer_canvas = outer_canvas;
        }
        // iterate through the music list and whenever we find a music that was marked for removal we check if that
        // current music is playing if it is playing then we halt it and set the playing status and the playing status
        // to false
        /*if(mplayer->music_list[i].remove) {
            TODO: REIMPLEMENT THIS
            if(Mix_PlayingMusic() && mplayer->playid == i) {
                Mix_HaltMusic();
//                mplayer->music_list[i].music_playing = false;
//                mplayer->current_music = NULL;
            }
            mplayer->music_list[i].remove = false;       
            mplayer_filemanager_freemusic(&mplayer->music_list[i]);
            continue;
        }*/
        /* whenever a music is being played that was not marked for removal we preserve the playid for that music by
           assigning a new playid for that currently playing music since we are deallocating the music list which can
           happen in any random position.
        */
        /*if(Mix_PlayingMusic() && mplayer->playid == i) {
            new_playid = j; preserve_playid = true;
        }*/

        /* As long as the file count in the current location index is greater than zero then we increment the file
           count by 1 each time we are iterating and also adding the new music path to the new music list
        */
        if(mplayer->locations[new_locationindex].file_count > 0) {
            mplayer->music_list[i].music_path = mplayer->locations[new_locationindex].files[new_location_filecount].path;
            new_location_filecount++;
        }

        // determine the music location index for the file
        mplayer->music_list[i].location_index = new_locationindex;
        // whenever the new location file count is equal to the number of files in that particular location
        // we increment the new location index and set the new_location_filecount equal to zero that way we can
        // determine which music location index for the music file
        if(new_location_filecount == mplayer->locations[new_locationindex].file_count) {
            new_locationindex++; new_location_filecount = 0;
        }
        mplayer->music_list[i].fit = true;
        /* Whenever removing a music we want to maintain the scroll position therefore we check if the current index i
           is equal to the mplayer->music_renderpos as long as that is true we set mplayer->music_renderpos equal to j
           which represents the new music render position after removal of the music location and its music files
        */
        mplayer_filemanager_copymusic_info(&mplayer->music_list[i], &newmusic_list[j++]);

        // calculate the canvas positions and dimensions for the current music that will not be removed
        utext.text_canvas.y += utext.text_canvas.h + 22;
        outer_canvas.y += utext.text_canvas.h + 25;
    }
    // check if the newmusic list is not NULL
    if(newmusic_list) {
        bool resume_playing = false;
        oldmusic_list = mplayer->music_list;
        // whenever we should preserve the playid for a currently playing music we changed the current playid to the new
        // playid and set the current music to link or point to the new music index at that particular playid
        if(!Mix_PausedMusic() && Mix_PlayingMusic()) {
            resume_playing = true;
            Mix_PauseMusic();
            printf("The music was paused for a moment due to the removal process\n");
        }
        if(preserve_playid) {
            // set the current music pointer to be equal to newmusic_list[new_playid] before chaning the mplayer->playid
  //          mplayer->current_music = &newmusic_list[new_playid];
            //mplayer->playid = new_playid;
        }
        
        if(resume_playing) {
            // add a slight delay to the music resumption process this will be suttle and create the illusion of
            // the music still playing
            SDL_Delay(10);
            printf("The music was resumed due to the removal process being completed\n");
            Mix_ResumeMusic();
        }
        mplayer->music_list = newmusic_list;
        mplayer->music_count = newmusic_count;
        mplayer->music_maxrenderpos = newmusic_count-1;
        free(oldmusic_list); oldmusic_list = NULL;
    }
    mplayer->musicpending_removalcount = 0;
}

void mplayer_filemanager_loadmusics(mplayer_t* mplayer) {
    if(!mplayer->total_filecount) {
        mplayer_filemanager_freemusic_list(mplayer);
        return;
    }
    music_t* music_list = NULL;
    size_t music_count = 0;
    Mix_Music* music = NULL;
    text_info_t utext = {18, NULL, NULL, white, {songs_box.x + 2, songs_box.y + 1}};
    SDL_Rect outer_canvas = utext.text_canvas;
    double music_durationsecs = 0;
    size_t startlocation_index = 0;
    if(mplayer->music_locationremoved) {
        mplayer_filemanager_removemusics_pendingremoval(mplayer);
        if(mplayer->musicsearchbar_data) {
            mplayer->music_newsearch = true;
        }
        return;
    }

    if(!mplayer->music_list) {
        music_list = calloc(mplayer->total_filecount, sizeof(music_t));
        // As long as the music count is greater than zero then we save the mplayer->music_list
        for(size_t i=0;i<mplayer->music_count;i++) {
            music_list[i] = mplayer->music_list[i];
        }
    } else if(mplayer->total_filecount > mplayer->music_count) {
        bool resume_music = false;
        if(Mix_PlayingMusic() && !Mix_PausedMusic()) {
            Mix_PauseMusic();
            resume_music = true;
        }
        printf("Reallocating memory\n");
        mplayer->music_list = (music_t*)realloc(mplayer->music_list, mplayer->total_filecount * sizeof(music_t));
        if(Mix_PlayingMusic()) {
//            mplayer->current_music = &mplayer->music_list[mplayer->playid];
        }
        if(mplayer->music_list == NULL) {
            printf("Realloc failed\n");
        }
        size_t size_diff = mplayer->total_filecount - mplayer->music_count;
        memset((music_t*)mplayer->music_list + mplayer->music_count, 0, size_diff * sizeof(music_t));
        music_list = mplayer->music_list;
        if(resume_music) {
            SDL_Delay(10);
            Mix_ResumeMusic();
        }
    }
    if(mplayer->music_locationadded) {
        music_count = mplayer->music_count;
        startlocation_index = mplayer->location_count-1;
    }
    for(size_t i=startlocation_index;i<mplayer->location_count;i++) {
        for(size_t j=0;j<mplayer->locations[i].file_count;j++) {
            music = music_list[music_count].music;
            if(!music) {
                #ifdef _WIN32
                char* music_path = mplayer_widetoutf8(mplayer->locations[i].files[j].path);
                #else
                char* music_path = mplayer->locations[i].files[j].path;
                #endif
                music = Mix_LoadMUS(music_path);
                if(music == NULL) {
                    #ifdef _WIN32
                    music = Mix_LoadMUS(mplayer->locations[i].files[j].altpath);
                    #endif
                    if(music == NULL) {
                        #ifdef _WIN32
                        printf("Error: Failed to load music %ls\n", mplayer->locations[i].files[j].path);
                        #else
                        printf("Error: Failed to load music %s\n", mplayer->locations[i].files[j].path);
                        #endif
                    }
                }
                #ifdef _WIN32
                free(music_path); music_path = NULL;
                #endif
                
                music_list[music_count].music = music;
                music_list[music_count].music_position.hrs = 0,
                music_list[music_count].music_position.mins = 0,
                music_list[music_count].music_position.secs = 0,
  
                music_durationsecs = Mix_MusicDuration(music);
                music_list[music_count].music_durationsecs = music_durationsecs;
                music_list[music_count].music_duration = mplayer_filemanager_music_gettime(music_durationsecs);
            }
            music_list[music_count].location_index = i;
            mplayer->locations[i].files[j].location_index = music_count;

            /* Render the music_name and initialize the other infos (positions, texture, etc) */
            if(!music_list[music_count].music_name) {
                music_list[music_count].music_name = mplayer_filemanager_getmusic_namefrompath(music, mplayer->locations[i].files[j].path);
            }
            if(mplayer->music_selectionmenu_checkbox_tickall) {
                music_list[music_count].fill = true;
                music_list[music_count].checkbox_ticked = true;
                mplayer->tick_count++;
            }
            utext.utext = music_list[music_count].music_name;
            utext.text_canvas = music_list[music_count].text_info.text_canvas;
            music_list[music_count].render = true;

            if(!music_list[music_count].text_texture) {
                music_list[music_count].text_texture = mplayer_textmanager_renderunicode(mplayer, mplayer->music_font, &utext);
            }
            outer_canvas.h = utext.text_canvas.h + 22, outer_canvas.w = WIDTH - scrollbar.w;
            utext.text_canvas.x = outer_canvas.x + 50,
            utext.text_canvas.y = outer_canvas.y + ((outer_canvas.h - utext.text_canvas.h) / 2);
            music_list[music_count].text_info = utext;
            music_list[music_count].outer_canvas = outer_canvas;
            music_list[music_count].fit = true;
            utext.text_canvas.y += utext.text_canvas.h + 22;
            outer_canvas.y += utext.text_canvas.h + 25;
            mplayer->music_maxrenderpos = music_count;
            if(!music_list[music_count].music_path) {
                music_list[music_count].music_path = mplayer->locations[i].files[j].path;
            }
            #ifdef _WIN32
            if(!music_list[music_count].music_alternatepath) {
                music_list[music_count].music_alternatepath = mplayer->locations[i].files[j].altpath;
            }
            #endif
            music_count++;
        }
    }
    // Before modifiying the music list ensure that the location that was added contains music files
    // to prevent crashing
    if(music_list) {
        mplayer->music_list = music_list;
        mplayer->music_count = mplayer->total_filecount;

        mplayer->music_lists[0] = mplayer->music_list;
        mplayer->music_counts[0] = mplayer->music_count;
        // as long as data is present in the search bar then we set new search equal to true so when we return back to the
        // default menu the music search results gets updated if a music location was removed vice versa.
        /*if(mplayer->musicsearchbar_data) {
            mplayer->music_newsearch = true;
            mplayer->update_searchresults = true;
        }*/
    }
}

#ifdef _WIN32
int CALLBACK set_browsertitle(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
    if (uMsg == BFFM_INITIALIZED)
      SetWindowTextA(hwnd, "Select Folder");
    return 0;
}
#endif

void mplayer_filemanager_browsefolder(mplayer_t* mplayer) {
    #ifdef _WIN32
    SDL_SysWMinfo info;
    SDL_GetVersion(&info.version);
    SDL_GetWindowWMInfo(mplayer->window, &info);
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
            mplayer_filemanager_addmusic_location(mplayer, folder_path);
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
        mplayer_filemanager_addmusic_location(mplayer, directory);
    }
    free(directory);
    #endif
}

char* mplayer_filemanager_getmusic_namefrompath(Mix_Music* music, void* path) {
    // If the music file path is NULL we just return NULL
    if(path == NULL) {
        return NULL;
    }
    char* music_name = NULL;
    size_t name_len = 0, ext_index = 0, byte_size = sizeof(char);
    #ifdef _WIN32
    wchar_t *wmusic_name = NULL, *filename = NULL, *filename_cp = NULL;
    wchar_t ext[6] = {0};
    wcscpy(ext, L".");
    wchar_t *(*string_concat)(wchar_t* dest, const wchar_t* src) = &wcscat; 
    filename = wcsrchr((wchar_t*)path, L'\\');
    byte_size = sizeof(wchar_t);
    #else
    char *filename = NULL, *filename_cp = NULL;
    char ext[6] = {0};
    strcpy(ext, ".");
    char *(*string_concat)(char* dest, const char* src) = &strcat;
    filename = strrchr((char*)path, '/');
    #endif
    // Whenever the last occurence of the character '\' in the file path is not found then filename will be equal to NULL
    // whenever if because NULL we exit the function by returning NULL
    if(!filename) {
        return NULL;
    }
    // SKip the character '\' assuming the filename will come after the '\' character
    filename++;
    #ifdef _WIN32
    name_len = wcslen(filename);
    #else
    name_len = strlen(filename);
    #endif
    int music_type = Mix_GetMusicType(music);
    switch(music_type) {
        case MUS_MP3:  string_concat(ext, FILE_EXTENSIONS[MPLAYER_MP3EXT]);  break;
        case MUS_FLAC: string_concat(ext, FILE_EXTENSIONS[MPLAYER_FLACEXT]); break;
        case MUS_OGG:  string_concat(ext, FILE_EXTENSIONS[MPLAYER_OGGEXT]);  break;
        case MUS_OPUS: string_concat(ext, FILE_EXTENSIONS[MPLAYER_OPUSEXT]); break;
        case MUS_WAV:  string_concat(ext, FILE_EXTENSIONS[MPLAYER_WAVEXT]);  break;
        default:       string_concat(ext, FILE_EXTENSIONS[MPLAYER_M4AEXT]);
    }
    for(size_t i=0;i<name_len;i++) {
        filename++;
        #ifdef _WIN32
        if(wcscmp(filename, ext) == 0) { ext_index = i; break; }
        #else
        if(strcmp(filename, ext) == 0) { ext_index = i; break; }
        #endif
    }
    if(ext_index == 0) {
        filename -= name_len, filename_cp = calloc(name_len+1, byte_size);
    } else {
        name_len = ext_index+1, filename -= name_len,
        filename_cp = calloc(name_len+1, byte_size);
    }
    for(size_t i=0;i<name_len;i++) {
        filename_cp[i] = filename[i];
    }
    /*
        On windows we need to convert wchar_t into a multibyte string in utf8 encoding
    */
    #ifdef _WIN32
    music_name = mplayer_widetoutf8(filename_cp);
    #else
    music_name = filename_cp;
    #endif
    return music_name;
}

mtime_t mplayer_filemanager_music_gettime(double seconds) {
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

void mplayer_filemanager_freemusic_searchresult(music_t** music_searchresult, size_t* music_resultcount) {
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

void mplayer_filemanager_freemusicpath_info(mplayer_t* mplayer) {
    for(size_t i=0;i<mplayer->location_count;i++) {
        for(size_t j=0;j<mplayer->locations[i].file_count;j++) {
            free(mplayer->locations[i].files[j].path); mplayer->locations[i].files[j].path = NULL;
            #ifdef _WIN32
            free(mplayer->locations[i].files[j].altpath); mplayer->locations[i].files[j].altpath = NULL;
            #endif
        }
        free(mplayer->locations[i].path); mplayer->locations[i].path = NULL;
        free(mplayer->locations[i].files); mplayer->locations[i].files = NULL;
        mplayer->locations[i].file_count = 0;
    }
    
    mplayer->total_filecount = 0;
    mplayer->location_count = 0;
    free(mplayer->locations); mplayer->locations = NULL;
}

void mplayer_filemanager_freemusic(music_t* music_ref) {
    // release the memory used for the music name, music name texture, and other music related data that was
    // allocaterd in memory
    SDL_DestroyTexture(music_ref->text_texture); music_ref->text_texture = NULL;
    free(music_ref->music_name); music_ref->music_name = NULL;
    music_ref->music_path = NULL;
    #ifdef _WIN32
    music_ref->music_alternatepath = NULL;
    #endif
    Mix_FreeMusic(music_ref->music); music_ref->music = NULL;
}

void mplayer_filemanager_freemusic_list(mplayer_t* mplayer) {
    /* Release the memory used by the music list back to the system */
    for(size_t i=0;i<mplayer->music_count;i++) {
        mplayer_filemanager_freemusic(&mplayer->music_list[i]);
    }
    //mplayer_filemanager_freemusic_searchresult(&mplayer->music_searchresult, &mplayer->music_searchresult_count);
    free(mplayer->music_list); mplayer->music_list = NULL; mplayer->music_count = 0;
    mplayer->musicpending_removalcount = 0;
    mplayer->music_renderpos = 0;
    mplayer->music_maxrenderpos = 0;
    //mplayer->match_maxrenderpos = 0;
    //mplayer->music_searchrenderpos = 0;
}