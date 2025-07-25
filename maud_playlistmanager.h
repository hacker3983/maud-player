#ifndef _MUSIC_PLAYLISTMANAGER_H
#define _MUSIC_PLAYLISTMANAGER_H
#include "maud_player.h"
#include "maud_selectionmenu.h"
#include "stringlist.h"
#include "maud_queue.h"
#include "maud_filemanager.h"
#include "maud_tooltips.h"

enum playlist_layoutmode {
    PLAYLIST_LISTVIEW,
    PLAYLIST_GRIDVIEW,
    PLAYLIST_LAYOUTCOUNT
};

void maud_playlistmanager_initialize_playlistinput(maud_t* maud);
void maud_playlistmanager_initialize_renameinput(maud_t* maud, const char* input_data);
SDL_Texture* maud_playlistmanager_getlayouticon(maud_t* maud, int type);
SDL_Texture* maud_playlistmanager_getcurrentlayouticon(maud_t* maud);
static char* maud_playlistmanager_getlayoutname(int type);
static char* maud_playlistmanager_getcurrentlayoutname(maud_t* maud);
void maud_playlistmanager_write_data_tofile(maud_t* maud);
void maud_playlistmanager_write_playlist_tofile(maud_t* maud, FILE* f, maud_playlist_t playlist);
void maud_playlistmanager_write_escapedstring_tofile(FILE* f, const char* string);
char* maud_playlistmanager_getmusicnamefrom_index(maud_t* maud, size_t music_listindex,
    size_t music_id);
void maud_playlistmanager_getmusicindex_byname(maud_t* maud, char* music_name,
    size_t* music_listindex, size_t* music_id);
void maud_playlistmanager_createplaylist_fromparsed_data(maud_t* maud, stringlist_t* string_list);
char* maud_playlistmanager_parsemusic_name(char* music_name, size_t* list_index);
char* maud_playlistmanager_getstring(FILE* playlist_datafile);
bool maud_playlistmanager_read_datafile(maud_t* maud);
bool maud_playlistmanager_createplaylist(maud_t* maud, const char* playlist_name);
void maud_playlistmanager_addmusic_toplaylist(maud_t* maud, const char* playlist_name,
    size_t music_listindex, size_t music_id);

void maud_playlistmanager_addmusicselection_toplaylist(maud_t* maud, const char* playlist_name);
bool maud_playlistmanager_removeplaylist(maud_t* maud, const char* playlist_name);
void maud_playlistmanager_renderplaylist_listlayout(maud_t* maud, size_t playlist_index,
    SDL_Rect* playlist_background);
void maud_playlistmanager_renderplaylist_gridlayout(maud_t* maud, int start_x,
    size_t playlist_index, SDL_Rect* playlist_background, bool* show_tooltip);

void maud_playlistmanager_renderplaylist_card(maud_t* maud, SDL_Rect* playlist_card);

// Find the maximum height from a giving list of rectangles
int maud_playlistmanager_findmaxheight(SDL_Rect* rects, size_t rect_count);

// Functions used for initializing button dimensions and positions on screen
void maud_playlistmanager_initialize_newplaylistbutton(maud_t* maud);
void maud_playlistmanager_initialize_playlistlayout_toggleswitch(maud_t* maud);
void maud_playlistmanager_initialize_buttonbar(maud_t* maud);
void maud_playlistmanager_setplaylist_cardposition(maud_t* maud, SDL_Rect playlistmenu_canvas,
    SDL_Rect* playlist_card);


// Display a toggle switch for changing the layout of how playlists are displayed on the screen
void maud_playlistmanager_display_playlistlayout_toggleswitch(maud_t* maud);
// Display the new playlist button
void maud_playlistmanager_display_newplaylistbutton(maud_t* maud);
// Diplay input for renaming playlists
void maud_playlistmanager_displayrename_input(maud_t* maud);
// Display the validation message on screen whenever playlist input field is empty
void maud_playlistmanager_display_playlistname_validation(maud_t* maud);
// Display all the button elements such as the new playlist button in the playlist tab
void maud_playlistmanager_display_buttonbar(maud_t* maud);
// Display playlists on screen
void maud_playlistmanager_displayplaylists(maud_t* maud);

// Display the playlist menu after we have clicked on a playlist
void maud_playlistmanager_display_playlistmenu(maud_t* maud, SDL_Rect* playlistmenu_canvasref);

// Display the contents of a playlist
void maud_playlistmanager_display_playlistcontent(maud_t* maud);

// Display all the elements in the playlist tab
void maud_playlistmanager_display(maud_t* maud);

void maud_playlistmanager_display_layoutdrop_downmenu(maud_t* maud);

void maud_playlistmanager_handle_playlistlayout_toggleswitch(maud_t* maud);

// Handles events for playlists that are rendered on the screen
void maud_playlistmanager_handleplaylist_event(maud_t* maud, size_t playlist_index, SDL_Rect playlist_canvas);

// Destroys the memory used by a playlist
void maud_playlistmanager_destroyplaylist(maud_playlist_t* playlist);

// Destroys / deallocate all the memory used by the playlist manager
void maud_playlistmanager_destroy(maud_t* maud);
#endif