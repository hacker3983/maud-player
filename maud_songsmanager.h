#ifndef _MUSIC_SONGSMANAGER_H
#define _MUSIC_SONGSMANAGER_H
#include "music_queue.h"
#include "music_player.h"
#include "music_checkboxes.h"
#include "music_selectionmenu.h"

int mplayer_songsmanager_playmusic(mplayer_t* mplayer);
void mplayer_songsmanager_playmusic_pause(mplayer_t* mplayer);
void mplayer_songsmanager_songstab_rendersongs(mplayer_t* mplayer);
void mplayer_songsmanager_addplayback_error(mplayer_t* mplayer, const char* music_name);
void mplayer_songsmanager_rendersong_canvas(mplayer_t* mplayer, music_t* music_list, size_t music_index);
void mplayer_songsmanager_handlesong_playbutton_hover(mplayer_t* mplayer, SDL_Rect outer_canvas);
void mplayer_songsmanager_handlesongselection(mplayer_t* mplayer, music_t* music_list, size_t music_id, text_info_t* music_textinfo);
void mplayer_songsmanager_handleplaybutton(mplayer_t* mplayer, music_t* music_list, size_t music_id);
void mplayer_songsmanager_handlecheckbox_musicselection(mplayer_t* mplayer, music_t* music_list, size_t music_id);
void mplayer_songsmanager_handleskipbutton(mplayer_t* mplayer);
void mplayer_songsmanager_handleprevbutton(mplayer_t* mplayer);
void mplayer_songsmanager_togglemusic_playback(mplayer_t* mplayer);
#endif