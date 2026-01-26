#include "maud_file.h"
#include "maud_string.h"
#include "maud_player.h"
#include <dirent.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
bool maud_file_init(maud_file_t* file, const char* path, const char* altpath) {
    char *path_dupstr = NULL, *altpath_dupstr = NULL;
    if(path) {
        path_dupstr = maud_dupstr(path, strlen(path));
        if(!path_dupstr) {
            return false;
        }
    }
    if(altpath) {
        altpath_dupstr = maud_dupstr(altpath, strlen(altpath));
        if(!altpath_dupstr) {
            free(path_dupstr);
            return false;
        }
    }
    file->path = path_dupstr;
    file->altpath = altpath_dupstr;
    return true;
}

#ifdef _WIN32
bool maud_file_path_exists(const wchar_t* location_path) {
    if(!PathFileExistsW(location_path)) {
        LPSTR messageBuffer = NULL;
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
            | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&messageBuffer, 0, NULL);
        fprintf(stderr, "Error: The music location %ls: %s\n", location_path, messageBuffer);
        LocalFree(messageBuffer);
        return false;
    }
    return true;
}
#else
bool maud_file_path_exists(const char* location_path) {
    DIR* dirp = opendir(location_path);
    if(!dirp) {
        fprintf(stderr, "Error: The music location %s: %s\n", location_path,
            strerror(errno));
        return false;
    }
    closedir(dirp);
    return true;
}
#endif

void maud_file_destroy(maud_file_t* file) {
    free(file->path);
    free(file->altpath);
    file->path = NULL;
    file->altpath = NULL;
}

void maud_file_list_init(maud_filelist_t* list) {
    list->files = NULL;
    list->count = 0;
}

void maud_file_list_addlocation(maud_filelist_t* list, const char* path, const char* altpath) {
    if(!path) {
        return;
    }
    maud_file_t new_file = {0};
    if(!maud_file_init(&new_file, path, altpath)) {
        return;
    }
    size_t new_filecount = list->count + 1;
    maud_file_t* new_files = realloc(list->files, new_filecount *
            sizeof(maud_file_t));
    if(!new_files) {
        maud_file_destroy(&new_file);
        return;
    }
    new_files[list->count] = new_file;
    list->files = new_files;
    list->count = new_filecount;
}

void maud_file_list_removelocation(maud_filelist_t* list, size_t location_index) {
    if(!list->files) {
        return;
    }
    if(location_index >= list->count) {
        return;
    }
    for(size_t i=location_index;i<list->count-1;i++) {
        maud_file_t current_file = list->files[i];
        list->files[i] = list->files[i+1],
        list->files[i+1] = current_file;
    }
    list->count--;
    maud_file_destroy(&list->files[list->count]);
    if(!list->count) {
        maud_file_list_init(list);
    }
    list->files = realloc(list->files, list->count * sizeof(maud_file_t));
}

void maud_file_list_load_files(maud_filelist_t* list, const char* location_path) {
    #ifdef _WIN32
    wchar_t *path = maud_stringtowide(location_path);
    size_t location_len = wcslen(path), path_patlen = location_len + 8;
    if(!maud_file_path_exists(path)) {
        free(path);
        return;
    }
    wchar_t* path_pattern = calloc(path_patlen, sizeof(wchar_t));
    WIN32_FIND_DATAW fd = {0};
    HANDLE hfind = INVALID_HANDLE_VALUE;
    for(int i=0;FILE_EXTENSIONS[i] != NULL;i++) {
        wchar_t* extension = maud_stringtowide(FILE_EXTENSIONS[i]);
        wcscpy(path_pattern, path);
        wcscat(path_pattern, L"\\*.");
        wcscat(path_pattern, extension);
        hfind = FindFirstFileW(path_pattern, &fd);
        if(hfind == INVALID_HANDLE_VALUE) {
            memset(path_pattern, 0, path_patlen);
            free(extension);
            continue;
        }
        do {
            size_t length_str = wcslen(fd.cFileName),
                   length_altstr = wcslen(fd.cAlternateFileName),
                   altpath_len = location_len + length_altstr + 7,
                   path_len = location_len + length_str + 7;

            wchar_t* new_wpath = calloc(path_len+1, sizeof(wchar_t));
            wcscpy(new_wpath, path);
            wcscat(new_wpath, L"\\");
            wcsncat(new_wpath, fd.cFileName, length_str);

            char *new_filepath = maud_widetoutf8(new_wpath);
            free(new_wpath);

            wchar_t* new_waltpath = calloc(altpath_len+1, sizeof(wchar_t));
            wcscpy(new_waltpath, path);
            wcscat(new_waltpath, L"\\");
            wcsncat(new_waltpath, fd.cAlternateFileName, length_altstr);

            char *new_filealt_path = maud_widetoutf8(new_waltpath);
            free(new_waltpath);
            maud_file_list_addlocation(list, new_filepath, new_filealt_path);
            free(new_filepath);
            free(new_filealt_path);
        } while(FindNextFileW(hfind, &fd));
        free(extension);
        FindClose(hfind);
        memset(path_pattern, 0, path_patlen);
    }
    free(path_pattern);
    free(path);
    #else
    char* path = location_path;
    if(!maud_file_path_exists(path)) {
        return;
    }
    DIR* dirp = opendir(path);
    struct dirent* entry = readdir(dirp);
    struct stat sb = {0};
    while(entry) {
        char* ext = strrchr(entry->d_name, '.');
        if(!ext) {
            entry = readdir(dirp); continue;
        }
        ext++;
        for(size_t j=0;FILE_EXTENSIONS[j] != NULL;j++) {
            if(strcmp(FILE_EXTENSIONS[j], ext) == 0) {
                char* new_path = NULL;
                maud_concatstr(&new_path, path);
                maud_concatstr(&new_path, "/");
                maud_concatstr(&new_path, entry->d_name);
                maud_file_list_addlocation(list, new_path, NULL);
                free(new_path);
                break;
        }
            }
            entry = readdir(dirp);
    }
    closedir(dirp);
    #endif
}

void maud_file_list_destroy(maud_filelist_t* list) {
    for(size_t i=0;i<list->count;i++) {
        maud_file_destroy(&list->files[i]);
    }
    free(list->files);
    maud_file_list_init(list);
}