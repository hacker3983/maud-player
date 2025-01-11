#ifndef _MUSIC_PLAYQUEUE_H
#define _MUSIC_PLAYQUEUE_H
#include "music_player.h"
#include "music_textmanager.h"

void mplayer_queue_init(music_queue_t* queue);
music_queueitem_t* mplayer_queue_realloclist_by(music_queue_t* queue, size_t amount);
size_t* mplayer_queue_reallocmusicid_listby(size_t** music_ids, size_t amount, size_t* music_count);
size_t mplayer_queue_getmusic_count(music_queue_t queue);
void mplayer_queue_swap(size_t* a, size_t* b);
void mplayer_queue_swapitem(music_queueitem_t* a, music_queueitem_t* b);
bool mplayer_queue_splitby_playid(music_queue_t* queue, size_t playindex, size_t playid);
bool mplayer_queue_addmusic(music_queue_t* queue, size_t music_listindex, size_t music_id);
bool mplayer_queue_addmusicqueue_toplaynext(mplayer_t* mplayer, music_queue_t* destination_queue, music_queue_t* source_queue);
bool mplayer_queue_addmusicfrom_queue(music_queue_t* destination_queue, music_queue_t* source_queue);
void mplayer_queue_removemusicby_id(music_queue_t* queue, size_t music_listindex, size_t music_id);
void mplayer_queue_display(mplayer_t* mplayer, music_queue_t queue);
void mplayer_queue_print(mplayer_t* mplayer, music_queue_t queue);
void mplayer_queue_destroy(music_queue_t* queue);
#endif