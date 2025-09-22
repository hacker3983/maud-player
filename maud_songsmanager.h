#ifndef _MAUD_SONGSMANAGER_H
#define _MAUD_SONGSMANAGER_H
#include "maud_queue.h"
#include "maud_player.h"
#include "maud_checkbox.h"
#include "maud_selectionmenu.h"

int maud_songsmanager_playmusic(maud_t* maud);
void maud_songsmanager_playmusic_pause(maud_t* maud);
void maud_songsmanager_songstab_rendersongs(maud_t* maud);
void maud_songsmanager_addplayback_error(maud_t* maud, const char* music_name);
void maud_songsmanager_rendersong_canvas(maud_t* maud, music_t* music_list, size_t music_index);
void maud_songsmanager_handlesong_playbutton_hover(maud_t* maud, SDL_Rect outer_canvas);
void maud_songsmanager_handlesongselection(maud_t* maud, music_t* music_list, size_t music_id, text_info_t* music_textinfo);
void maud_songsmanager_handleplaybutton(maud_t* maud, music_t* music_list, size_t music_id);
void maud_songsmanager_handlecheckbox_musicselection(maud_t* maud, music_t* music_list, size_t music_id);
void maud_songsmanager_handleskipbutton(maud_t* maud);
void maud_songsmanager_handleprevbutton(maud_t* maud);
void maud_songsmanager_togglemusic_playback(maud_t* maud);
#endif