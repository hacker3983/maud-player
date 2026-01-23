#ifndef _MAUD_LOCATION_H
#define _MAUD_LOCATION_H
#include "maud_file.h"
#include "maud_player.h"

bool maud_location_init(maud_location_t* location, const char* path);
void maud_location_destroy(maud_location_t* location);

void maud_location_list_init(maud_locationlist_t* list);
bool maud_location_list_locationexists(maud_locationlist_t* list, const char* path);
void maud_location_list_addlocation(maud_locationlist_t* list, const char* path);
void maud_location_list_removelocation(maud_locationlist_t* list, size_t location_index);
void maud_location_list_load_data(maud_locationlist_t* list);
void maud_location_list_write_data(maud_locationlist_t* list);
void maud_location_list_destroy(maud_locationlist_t* list);
#endif