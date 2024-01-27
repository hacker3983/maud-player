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
    char* music_loc = calloc(2, sizeof(char)), c = '\0';
    size_t mloc_len = 0, muslist_count = 0;
    while((c = fgetc(f)) != EOF) {
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
        strncpy(music_loc, temp, mloc_len);
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
        size_t location_len = strlen(mplayer->musinfo.locations[i].path);
        size_t pathpat_len = location_len + 8;
        #ifdef _WIN32
        char* path_pattern = calloc(pathpat_len, sizeof(char));
        for(int j=0;FILE_EXTENSIONS[j] != NULL;j++) {
            strcpy(path_pattern, mplayer->musinfo.locations[i].path);
            strcat(path_pattern, "\\*.");
            strcat(path_pattern, FILE_EXTENSIONS[j]);
            WIN32_FIND_DATA fd = {0};
            HANDLE hfind = FindFirstFile(path_pattern, &fd);
            if(hfind == INVALID_HANDLE_VALUE) {
                printf("No file extension found\n");
                continue;
            }
            do {
                music_files[mfile_count].path = calloc(location_len + strlen(fd.cFileName) + 7, sizeof(char));
                strcpy(music_files[mfile_count].path, mplayer->musinfo.locations[i].path);
                strcat(music_files[mfile_count].path, "\\");
                strcat(music_files[mfile_count].path, fd.cFileName);
                printf("%d %s\n", i, fd.cFileName);
                mfile_count++;
                music_files = realloc(music_files, (mfile_count + 1) * sizeof(musloc_t));
                music_files[mfile_count].path = NULL;
            } while(FindNextFile(hfind, &fd));
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
            char* ext = strrchr(entry->d_name, '.');
            size_t total_mpath_size = location_len + strlen(entry->d_name) + 2;
            // use stat instead of d_type to make more filesystem independent
            stat(entry->d_name, &sb);
            if(strcmp(ext, entry->d_name) != 0 && S_ISREG(sb.st_mode)) {
                for(size_t j=0;FILE_EXTENSIONS[j] != NULL;j++) {
                    if(strcmp(ext, FILE_EXTENSIONS[j]) == 0) {
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
        fputs("\n", f);
    }
    fwrite(location, sizeof(char), strlen(location), f);
    fclose(f);
    mplayer_freemusic_info(mplayer);
    mplayer_getmusicpath_info(mplayer);
}

void mplayer_loadmusics(mplayer_t* mplayer) {
    music_t* music_list = calloc(mplayer->musinfo.file_count, sizeof(music_t));
    Mix_Music* music = NULL;
    double music_durationsecs = 0;
    for(size_t i=0;i<mplayer->musinfo.file_count;i++) {
        music = Mix_LoadMUS(mplayer->musinfo.files[i].path);
        music_durationsecs = Mix_MusicDuration(music);

        music_list[i].music = music;
        music_list[i].music_name = mplayer_getmusic_namefrompath(mplayer->musinfo.files[i].path);
        music_list[i].music_path = mplayer->musinfo.files[i].path;
        music_list[i].music_position.hrs = 0, music_list[i].music_position.mins = 0,
        music_list[i].music_position.secs = 0;
        music_list[i].music_playing = 0;
        music_list[i].music_durationsecs = music_durationsecs;
        music_list[i].music_duration = mplayer_music_gettime(music_durationsecs);
    }
    mplayer->music_list = music_list;
}

char* mplayer_getmusic_namefrompath(const char* path) {
    char* music_name = NULL, *filename = NULL, *ext = NULL;
    ext = strrchr(path, '.');
    #ifdef _WIN32
    filename = strrchr(path, '\\');
    #else
    filename = strrchr(path, '/');
    #endif
    size_t name_len = (strlen(filename) - strlen(ext));
    music_name = calloc(name_len + 1, sizeof(char));
    for(size_t i=1;i<name_len;i++) {
        music_name[i-1] = filename[i];
    }
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
        free(mplayer->music_list[i].music_name);
        Mix_FreeMusic(mplayer->music_list[i].music);
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