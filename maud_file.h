#ifndef _MAUD_FILE_H
#define _MAUD_FILE_H
#include "maud_locationdef.h"

bool maud_file_init(maud_file_t* file, const char* path, const char* altpath);
#ifdef _WIN32
bool maud_file_path_exists(const wchar_t* location_path);
#else
bool maud_file_path_exists(const char* location_path);
#endif
void maud_file_destroy(maud_file_t* file);

void maud_file_list_init(maud_filelist_t* list);
void maud_file_list_load_files(maud_filelist_t* list, const char* location_path);
void maud_file_list_addlocation(maud_filelist_t* list, const char* path);
void maud_file_list_removelocation(maud_filelist_t* list, size_t location_index);
void maud_file_list_destroy(maud_filelist_t* list);
#endif